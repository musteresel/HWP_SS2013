/* Multitasking implementation
 * */
//-----------------------------------------------------------------------------
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdint.h>
//-----------------------------------------------------------------------------
#include "kernel/task.h"
#include "kernel/_port.h"
//-----------------------------------------------------------------------------
#define COUNT_MILLISECOND ((F_CPU/TASKTIMER_PRESCALER)/1000u) // with 8Mhz => 125
#define MAX_RR_TIME 10u // MilliSeconds
#define MAX_DELAY_TIME ((65536u/COUNT_MILLISECOND)- 1u) // MilliSeconds
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
static uint8_t idleTaskStack[50];
static Task idleTask;
//-----------------------------------------------------------------------------
void Task_waitCurrent(time_t delay) __attribute__ (( hot ));
void _waitCurrent_inner(void) __attribute__ (( naked, noinline, hot ));
static void idleTaskFct(void) __attribute__ (( noreturn, cold ));
void Multitasking_init(void) __attribute__ (( naked, cold ));
void Task_init(Task * task, TaskFct function, uint8_t * stack);
void _Task_setReady(Task * task);
void _Task_setNotReady(Task * task) __attribute__ (( hot ));
Task * _Task_getNextReady(void) __attribute__ (( hot ));
time_t _Task_enforceTimeslice(void) __attribute__ (( hot ));
//-----------------------------------------------------------------------------
__attribute__(( flatten, hot )) ISR(TIMER_ISR, ISR_NAKED)
{
	// Save the context of the current task
	port_SAVE_CONTEXT();
	// Update time information
	timeInfo.current = timeInfo.next;
	taskInfo.current->rrTime += timeInfo.current - timeInfo.taskStart;
	// Wake tasks whose time has come
	while (taskInfo.nextToWake &&
			timeInfo.current == taskInfo.nextToWake->wakeTime)
	{
		Task * toWake = taskInfo.nextToWake;
		taskInfo.nextToWake = toWake->next;
		if (toWake->priority < taskInfo.current->priority)
		{
			taskInfo.current = toWake;
		}
		_Task_setReady(toWake);
	}
	// Enforce equal timeslice scheduling
	time_t delay = _Task_enforceTimeslice();
	// Check for wake "event" coming before next timeslice event
	if (taskInfo.nextToWake)
	{
		time_t wakeDelay = taskInfo.nextToWake->wakeTime - timeInfo.current;
		if (wakeDelay < delay)
		{
			delay = wakeDelay;
		}
	}
	// Set timer
	OCR1A = delay * COUNT_MILLISECOND;
	// Update time information
	timeInfo.next += delay;
	timeInfo.taskStart = timeInfo.current;
	// Restore context of task to run
	port_RESTORE_CONTEXT();
	// Return and enable interrupts
	reti();
}
//-----------------------------------------------------------------------------
void Task_waitCurrent(time_t delay)
{
	cli();
	taskInfo.current->wakeTime = delay;
	_waitCurrent_inner();
}
void _waitCurrent_inner(void)
{
	// Save context
	port_SAVE_CONTEXT();
	// Get passed parameter
	time_t delay = taskInfo.current->wakeTime;
	// Estimate the current time and remaining delay
	// TODO: improve estimation
	time_t current = timeInfo.current + TCNT1 / COUNT_MILLISECOND;
	time_t remainingDelay = timeInfo.next - current;
	// Remove the current task from the ready list
	_Task_setNotReady(taskInfo.current);
	Task * toWait = taskInfo.current;
	toWait->wakeTime = current + delay;
	toWait->rrTime += current - timeInfo.taskStart;
	// Find next ready task
	timeInfo.taskStart = current;
	taskInfo.current = _Task_getNextReady();
	time_t rrDelay = _Task_enforceTimeslice();
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
	// Determine shortest delay
	if (rrDelay < delay)
	{
		delay = rrDelay;
	}
	// If neccessary, reset the timer
	if (delay < remainingDelay)
	{
		TCNT1 = 0;
		OCR1A = delay * COUNT_MILLISECOND;
		timeInfo.next = current + delay;
	}
	// Switch to the ready task with now highest priority
	port_RESTORE_CONTEXT();
	reti();
}
//-----------------------------------------------------------------------------
static void idleTaskFct(void)
{
	for (;;)
	{
		// TODO: will cause problems with other tasks accessing MCUCR
		MCUCR |= (1 << SE);
		sleep_cpu();
	}
}
//-----------------------------------------------------------------------------
void Multitasking_init(void)
{
	// Prepare the idle task
	idleTask.priority = 7;
	Task_init(&idleTask, idleTaskFct, &(idleTaskStack[49]));
	_Task_setReady(&idleTask);
	// Set the timer to CTC with prescaler 64
	TCCR1B = 0;
	TCNT1 = 0;
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
	// Start at time 0
	timeInfo.taskStart = 0;
	timeInfo.current = 0;
	// Switch to the task with highest priority
	taskInfo.current = _Task_getNextReady();
	// Apply timeslice and compute next time
	timeInfo.next = timeInfo.current + _Task_enforceTimeslice();
	// Set timer
	OCR1A = COUNT_MILLISECOND * timeInfo.next;
	// Enable the timer interrupt
	TIMSK1 |= (1 << OCIE1A);
	// "Restore" context
	port_RESTORE_CONTEXT();
	// Enable interrupts during switch
	reti();
}
//-----------------------------------------------------------------------------
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
		*stack-- = iterator;
	}
	// Save the pointer to be able to restore it
	task->sp = stack;
	// The task has not run, so it's timeslice usage is 0
	task->rrTime = 0;
}
void _Task_setReady(Task * task)
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
void _Task_setNotReady(Task * task)
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
Task * _Task_getNextReady(void)
{
	if (taskInfo.ready[0]) return taskInfo.ready[0];
	if (taskInfo.ready[1]) return taskInfo.ready[1];
	if (taskInfo.ready[2]) return taskInfo.ready[2];
	if (taskInfo.ready[3]) return taskInfo.ready[3];
	if (taskInfo.ready[4]) return taskInfo.ready[4];
	if (taskInfo.ready[5]) return taskInfo.ready[5];
	if (taskInfo.ready[6]) return taskInfo.ready[6];
	return taskInfo.ready[7];
}
time_t _Task_enforceTimeslice(void)
{
	time_t rrDelay;
	if (taskInfo.current->next == taskInfo.current)
	{
		// Only 1 task on priority, so no timeslicing needed
		taskInfo.current->rrTime = 0;
		rrDelay = MAX_DELAY_TIME;
	}
	else
	{
		while (taskInfo.current->rrTime >= MAX_RR_TIME)
		{
			taskInfo.current->rrTime = 0;
			taskInfo.ready[taskInfo.current->priority] = taskInfo.current->next;
			taskInfo.current = taskInfo.current->next;
		}
		rrDelay = MAX_RR_TIME - taskInfo.current->rrTime;
	}
	return rrDelay;
}

