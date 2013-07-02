#include <avr/interrupt.h>
#include <stdint.h>
#include "kernel/task.h"
#include "kernel/semaphore.h"



//-----------------------------------------------------------------------------
uint8_t Semaphore_try(Semaphore * semaphore)
{
	uint8_t sreg = SREG;
	cli();
	if (semaphore->count > 0)
	{
		semaphore->count--;
		SREG = sreg;
		return 1;
	}
	SREG = sreg;
	return 0;
}




void Semaphore_wait(Semaphore * semaphore)
{
	uint8_t sreg = SREG;
	cli();
	if (semaphore->count > 0)
	{
		semaphore->count--;
		SREG = sreg;
	}
	else
	{
		Task * current = Task_getCurrent();
		Task * prev = 0;
		Task * it = semaphore->waiting;
		while (it)
		{
			prev = it;
			it = it->next;
		}
		if (prev)
		{
			prev->next = current;
		}
		else
		{
			semaphore->waiting = current;
		}
		Task_setNotReady(current);
		current->next = 0;
		Task_yield();
	}
}




void Semaphore_signal(Semaphore * semaphore)
{
	uint8_t sreg = SREG;
	cli();
	if (semaphore->waiting)
	{
		Task * waiter = semaphore->waiting;
		semaphore->waiting = waiter->next;
		Task_setReady(waiter);
		Task_yield();
	}
	else
	{
		semaphore->count++;
		SREG = sreg;
	}
}




void Semaphore_init(Semaphore * semaphore, uint8_t initCount)
{
	semaphore->count = initCount;
	semaphore->waiting = 0;
}

