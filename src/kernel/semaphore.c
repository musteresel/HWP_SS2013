#include <avr/interrupt.h>
#include <stdint.h>
#include "kernel/task.h"
#include "kernel/semaphore.h"



//-----------------------------------------------------------------------------
void Semaphore_wait(Semaphore * semaphore)
{
	cli();
	if (semaphore->count > 0)
	{
		semaphore->count--;
		sei();
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
		sei();
	}
}




void Semaphore_init(Semaphore * semaphore, uint8_t initCount)
{
	semaphore->count = initCount;
	semaphore->waiting = 0;
}

