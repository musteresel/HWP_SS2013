#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>



#define PRESCALER 64
#define COUNT_MILLISECOND ((F_CPU/PRESCALER)/1000) // with 8Mhz => 125

#define MAX_RR_TIME 10 // MilliSeconds
#define MAX_DELAY_TIME ((65536/COUNT_MILLISECOND)- 1) // MilliSeconds
#define STACK_SIZE 128

typedef uint16_t time_t;

typedef struct __Task_struct
{
	uint8_t sreg; // Status Register
	uint8_t * sp; // Stack pointer
	uint8_t stack[STACK_SIZE]; // Tasks stack
	time_t rrTime; // Tasks run time (for round robin)
	uint8_t priority; // Task priority (0 - 7)
	time_t wakeTime;
	struct __Task_struct * next;
} Task;


Task * readyTasks[8];
Task * currentTask;
Task * nextTaskToWake;
time_t nextTime;
time_t taskStartTime;



void dispatch(void) __attribute__(( naked ));
void dispatch(void)
{
	PUSH_REGS();
	currentTask->sreg = SREG;
	currentTask->sp = (uint8_t *)(SP);
	uint8_t iterator = 0;
	do
	{
		currentTask = readyTasks[iterator++];
	} while (!currentTask);
	SREG = currentTask->sreg;
	SP = (uint16_t)(currentTask->sp);
	POP_REGS();
	asm volatile ("reti"); // TODO
}


ISR(TIMER_ISR)
{
	time_t currentTime;
	time_t timerDelay;
	time_t needDispatch = 0;
	// Assume nextTime is correct
	currentTime = nextTime;
	// Save the time the current task has run so far, in order to be fair
	// about rr scheduling
	currentTask->rrTime += currentTime - taskStartTime;
	// If the max rr time has been exceeded, we probably must take action
	if (currentTask->rrTime >= MAX_RR_TIME)
	{
		if (currentTask->next == currentTask)
		{
			// If there is no other task to do rr with, stick with this one
			// for as long as possible
			timerDelay = MAX_DELAY_TIME;
			taskStartTime = currentTime; // simulate dispatch
		}
		else
		{
			// Select the next task with the same priority and compute the
			// time until its rr time will be exceeded
			readyTasks[currentTask->priority] = currentTask->next;
			timerDelay = MAX_RR_TIME - currentTask->next->rrTime; // TODO assert > 0+x
			needDispatch = 1;
		}
		currentTask->rrTime = 0; // Eventually solves above assertion
	}
	if (nextTaskToWake && currentTime == nextTaskToWake->wakeTime)
	{
		// A task needs to be woken
		Task * toWake = nextTaskToWake;
		nextTaskToWake = nextTaskToWake->next;
		// Only dispatch if priority is higher than the current
		if (toWake->priority < currentTask->priority)
		{
			needDispatch = 1;
		}
		// Insert task into readyTasks
		if (readyTasks[toWake->priority])
		{
			// Insert at top of existing queue (TODO unfair?)
			Task * currentReady = readyTasks[toWake->priority];
			toWake->next = currentReady->next;
			currentReady->next = toWake;	
			readyTasks[toWake->priority] = toWake; // ?
		}
		else
		{
			// Insert as new queue
			readyTasks[toWake->priority] = toWake;
			toWake->next = toWake;
		}
	}
	if (nextTaskToWake)
	{
		// Adjust timerDelay to wake next task
		time_t wakeDelay = nextTaskToWake->wakeTime - currentTime;
		if (wakeDelay < timerDelay)
		{
			timerDelay = wakeDelay;
		}
	}
	// Ensure the next ISR knows the correct time
	nextTime += timerDelay;
	// set Timer TODO
	if (needDispatch)
	{
		// Only dispatch if neccessary
		dispatch();
		// Won't return here, so NO correct stack?
	}
}







/*
Task * currenTask;
Task * nextTaskToWake;
ISR(TIMER_ISR) // NOT naked, don't need to save ALL regs if not dispatching
{
	uint8_t currentTime = nextTime;
	uint8_t rrInterval;
	uint8_t timerDelay;
	// Save the time the current task has run so far, in order to be fair
	// about rr scheduling
	currentTask->rrTime += currentTime - taskStartTime;
	// Compute the time the next rr scheduling should happen
	rrInterval = MAX_RR_TIME - currentTask->rrTime;
	if (rrInterval == 0)
	{
		rrInterval = MAX_RR_TIME;
	}
	if (nextTaskToWake && currentTime == nextTaskToWake->wakeTime)
	{
		Task * toWake = nextTaskToWake;
		nextTaskToWake = nextTaskToWake->next;
		wake(toWake);
		if (nextTaskToWake)
		{
			timerDely = MIN(rrInterval, nextTaskToWake->wakeTime

	}

	if (nextTaskToWake)
	{
		timerDelay = MIN(rrInterval, nextTaskToWake->wakeTime - currentTime);
	}
	else
	{
		timerDelay = rrInterval;
	}
}}
*/


