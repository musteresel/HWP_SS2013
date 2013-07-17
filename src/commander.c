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
			if (packetBuffer[0] == 24)
			{
				current += 50;
			}
			else if (packetBuffer[0] == 23)
			{
				current -= 50;
			}
			else if (packetBuffer[0] == 22)
			{
				current = 0;
				WheelDistance dist = Incremental_getDistance();
				Communication_log(0,"L: %d R: %d", dist.left, dist.right);	
			}
			WheelSpeed speed;
			speed.left = current;
			speed.right = current;
			Speed_setDesired(&speed);
		}
		Task_waitCurrent(222);
	} while (1);
}
