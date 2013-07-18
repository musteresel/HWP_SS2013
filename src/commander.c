#include <stdint.h>
#include "kernel/task.h"
#include "communication.h"
#include "speed.h"
#include "sensor/incremental.h"

TASK_STATIC(commander,4,commanderFct,200,1);

static uint8_t packetBuffer[32];

static int16_t current;

static void commanderFct(void)
{
	uint8_t size;
	current = 0;
	do
	{
		size = Communication_readPacket(packetBuffer, 32);
		if (size)
		{
			if (packetBuffer[0] == 2)
			{
				Translation * t = (Translation *)(&packetBuffer[1]);
				Translation_set(t);
				Communication_log(0,"speed: %d, steering: %d", t->speed, t->steering);
			}
		}
		Task_waitCurrent(222);
	} while (1);
}
