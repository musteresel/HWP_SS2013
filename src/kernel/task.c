/* Multitasking implementation
 * */
//-----------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
#include "kernel/task.h"
#include "kernel/_port.h"
#include "util/attribute.h"




//-----------------------------------------------------------------------------
#define TIMER_COUNT_MS ((F_CPU/TASKTIMER_PRESCALER)/1000u) // with 8Mhz => 125
#define TASK_TIMESLICE_MS 10u // MilliSeconds
#define TIMER_COUNT_MAX_MS ((65536u/TIMER_COUNT_MS)- 1u) // MilliSeconds




//-----------------------------------------------------------------------------
#define TIMER_ISR TIMER1_COMPA_vect




//-----------------------------------------------------------------------------
static struct __TaskInfo
{
	Task * current;
	Task * nextToWake;
	Task * ready[8];
} taskInfo = { .nextToWake = 0, .ready = {0} };




static struct __TimeInfo
{
	time_t current;
	time_t next;
	time_t taskStart;
} timeInfo;




//-----------------------------------------------------------------------------
static uint8_t idleTaskStack[50];
static Task idleTask;




//-----------------------------------------------------------------------------
static void idleTaskFct(void)
{
	for (;;)
	{
		sleep_cpu();
	}
}




//-----------------------------------------------------------------------------
time_t Task_enforceTimeslice(uint8_t priority)
{
	Task * task = taskInfo.ready[priority];
	time_t delay;
	if (task->next == task)
	{
		task->rrTime = 0;
		delay = TIMER_COUNT_MAX_MS;
	}
	else
	{
		while (task->rrTime >= TASK_TIMESLICE_MS)
		{
			task->rrTime = 0;
			task = task->next;
		}
		taskInfo.ready[priority] = task;
		delay = TASK_TIMESLICE_MS - task->rrTime;
	}
	return delay;
}




Task * Task_getNextReady()
{
	uint8_t priority;
	if (taskInfo.ready[0]) priority = 0;
	else if (taskInfo.ready[1]) priority = 1;
	else if (taskInfo.ready[2]) priority = 2;
	else if (taskInfo.ready[3]) priority = 3;
	else if (taskInfo.ready[4]) priority = 4;
	else if (taskInfo.ready[5]) priority = 5;
	else if (taskInfo.ready[6]) priority = 6;
	else priority = 7;
	taskInfo.ready[priority]->wakeTime = Task_enforceTimeslice(priority);
	return taskInfo.ready[priority];
}




void Task_setReady(Task * task)
{
	if (taskInfo.ready[task->priority])
	{
		task->next = taskInfo.ready[task->priority]->next;
		taskInfo.ready[task->priority]->next = task;
		taskInfo.ready[task->priority] = task;
		// TODO: could cause starvation
	}
	else
	{
		taskInfo.ready[task->priority] = task;
		task->next = task;
	}
}




void Task_setNotReady(Task * task)
{
	if (task->next == task)
	{
		taskInfo.ready[task->priority] = 0;
	}
	else
	{
		// NOTE: increases performance, adds jitter
		Task * findPrevious = taskInfo.ready[task->priority];
		while (findPrevious->next != task)
		{
			findPrevious = findPrevious->next;
		}
		findPrevious->next = task->next;
		if (taskInfo.ready[task->priority] == task)
		{
			taskInfo.ready[task->priority] = findPrevious;
		}
	}
}




Task * Task_getCurrent(void)
{
	return taskInfo.current;
}




void Task_init(Task * task, TaskFct function, uint8_t * stack)
{
	// The "bottom" byte to be in use by the task
	stack--;
	// Return address as if pushed by an interrupt
	*stack-- = ((uint16_t)function) & 0xFF;
	*stack-- = ((uint16_t)function) >> 8;
	// Register R0 and SREG
	*stack-- = 0;
	*stack-- = 0;
	// Registers R31 to R1
	uint8_t iterator = 31;
	for (; iterator > 0; iterator--)
	{
		*stack-- = 0;
	}
	// Save the pointer to be able to restore it
	task->sp = stack;
	// The task has not run, so it's timeslice usage is 0
	task->rrTime = 0;
}




ATTRIBUTE( flatten ) ISR(TIMER_ISR, ISR_NAKED)
{
	// Save the context of the current task. The current instruction
	// pointer has been saved on the stack by the processor
	port_SAVE_CONTEXT();
	// Update time information
	timeInfo.current = timeInfo.next;
	taskInfo.current->rrTime += timeInfo.current - timeInfo.taskStart;
	// Wake tasks which are ready
	while (taskInfo.nextToWake &&
			timeInfo.current == taskInfo.nextToWake->wakeTime)
	{
		Task * toWake = taskInfo.nextToWake;
		taskInfo.nextToWake = toWake->next;
		if (toWake->priority < taskInfo.current->priority)
		{
			taskInfo.current = toWake;
		}
		Task_setReady(toWake);
	}
	// Enforce equal timeslice scheduling on priority level
	time_t delay = Task_enforceTimeslice(taskInfo.current->priority);
	// Check for wake "event" coming before next timeslice event
	if (taskInfo.nextToWake)
	{
		time_t wakeDelay = taskInfo.nextToWake->wakeTime - timeInfo.current;
		if (wakeDelay < delay)
		{
			delay = wakeDelay;
		}
	}
	// Set the timer to fire at the correct time
	OCR1A = delay * TIMER_COUNT_MS;
	// Update time information
	timeInfo.next += delay;
	timeInfo.taskStart = timeInfo.current;
	// Restore context of task to run
	port_RESTORE_CONTEXT();
	// Return and enable interrupts
	reti();
}




ATTRIBUTE( naked, noinline ) static void waitCurrent_inner(void)
{
	// Save the current tasks context. This assumes that a return address
	// is on the top of the stack!
	port_SAVE_CONTEXT();
	// Get hacky passed parameter
	time_t delay = taskInfo.current->wakeTime;
	// Calculate time information
	time_t offset = TCNT1 / TIMER_COUNT_MS;
	time_t current = timeInfo.current + offset;
	time_t remainingDelay = timeInfo.next - current;
	// Remove the current task from the ready list and save its stats
	Task * toWait = taskInfo.current;
	Task_setNotReady(toWait);
	toWait->wakeTime = current + delay;
	toWait->rrTime += current - timeInfo.taskStart;
	// Insert the task in the wait list
	Task * last = 0;
	Task * iterator = taskInfo.nextToWake;
	while (iterator && (iterator->wakeTime - current) < delay)
	{
		last = iterator;
		iterator = iterator->next;
	}
	if (last)
	{
		last->next = toWait;
	}
	else
	{
		taskInfo.nextToWake = toWait;
	}
	toWait->next = iterator;
	// Select a new task and recalculate timer value
	timeInfo.taskStart = current;
	taskInfo.current = Task_getNextReady();
	if (taskInfo.current->wakeTime < delay)
	{
		delay = taskInfo.current->wakeTime;
	}
	if (delay < remainingDelay)
	{
		OCR1A = (offset + delay) * TIMER_COUNT_MS;
		timeInfo.next = current + delay;
	}
	// Restore the context and return to the now running task
	port_RESTORE_CONTEXT();
	reti();
}




void Task_waitCurrent(time_t delay)
{
	taskInfo.current->wakeTime = delay;
	waitCurrent_inner();
}




ATTRIBUTE( naked ) void Multitasking_init(void)
{
	// Prepare the idle task
	idleTask.priority = 7;
	Task_init(&idleTask, idleTaskFct, &(idleTaskStack[49]));
	Task_setReady(&idleTask);
	// Start at time 0
	timeInfo.taskStart = 0;
	timeInfo.current = 0;
	// Switch to the task with highest priority
	taskInfo.current = Task_getNextReady();
	// Apply timeslice and compute next time
	timeInfo.next = timeInfo.current + taskInfo.current->wakeTime;
	// Set timer
	OCR1A = TIMER_COUNT_MS * timeInfo.next;
	// Enable the timer interrupt
	TIMSK1 |= (1 << OCIE1A);
	// Set the timer to CTC with prescaler 64
	TCCR1B = 0;
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
	// "Restore" context
	port_RESTORE_CONTEXT();
	// Enable interrupts during switch
	reti();
}




ATTRIBUTE( naked ) void Task_yield(void)
{
	port_SAVE_CONTEXT();
	cli();
	time_t offset = TCNT1 / TIMER_COUNT_MS;
	taskInfo.current->rrTime += offset;
	taskInfo.current = Task_getNextReady();
	time_t remainingDelay = timeInfo.next - timeInfo.current;
	time_t delay = taskInfo.current->wakeTime;
	if (delay < remainingDelay)
	{
		OCR1A = (offset + delay) * TIMER_COUNT_MS;
		timeInfo.next = timeInfo.current + offset + delay;
	}
	port_RESTORE_CONTEXT();
	reti();
}

