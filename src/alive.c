#include <stdint.h>
#include "kernel/task.h"
#include "communication.h"



TASK_STATIC(alive,5,aliveFct,100,1);


static void aliveFct(void)
{
	uint8_t aliveCount = 1;
	Task_waitCurrent(113);
	do
	{
		Task_waitCurrent(1000);
		Communication_log(COMMUNICATION_FINE, "Alive: %u", aliveCount);
		aliveCount++;
	} while (1);
}

