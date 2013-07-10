#include <stdint.h>
#include "kernel/task.h"
#include "communication.h"




TASK_STATIC(commander,4,commanderFct,200,1);

static uint8_t packetBuffer[32];

static void commanderFct(void)
{
	uint8_t size;
	do
	{
		size = Communication_readPacket(packetBuffer, 32);
		if (size)
		{
			Communication_log(COMMUNICATION_FINE,
					"[COMMANDER] Received packet on channel %u (size %u byte)",
					packetBuffer[0], size);
		}
		Task_waitCurrent(222);
	} while (1);
}
