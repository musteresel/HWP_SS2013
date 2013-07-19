#include <stdint.h>
#include <math.h>
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
				Translation t;
				int16_t * data = (int16_t*)&(packetBuffer[1]);
				t.angle = 360 - (180 * atan2(data[0],data[1])) / 3.141592654;
				if (t.angle > 360)
				{
					t.angle -= 360;
				}
				t.length = sqrt((double)data[0] * data[0] + (double)data[1] * data[1]);
				Translation_apply(t);
				Communication_log(0,"length: %d, angle: %d", t.length, t.angle);
			}
		}
		Task_waitCurrent(222);
	} while (1);
}
