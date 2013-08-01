#include <stdint.h>
#include <math.h>
#include "kernel/task.h"
#include "communication.h"
#include "speed.h"
#include "sensor/incremental.h"
#include "obstacleavoidance.h"


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
			if (packetBuffer[0] == 22)
			{
				int32_t * target = (int32_t *)&(packetBuffer[1]);
				Waypoint wp;
				wp.x = target[0];
				wp.y = target[1];
				Communication_log(0,"To (%d | %d)",wp.x,wp.y);
				Pathtracking_addWaypoint(&wp);
			}
			if (packetBuffer[0] == 2)
			{
				Translation t;
				int16_t * data = (int16_t*)&(packetBuffer[1]);
				t.angle = 360 + (180 * atan2(data[1],data[0])) / 3.141592654;
				if (t.angle > 360)
				{
					t.angle -= 360;
				}
				t.length = sqrt((double)data[0] * data[0] + (double)data[1] * data[1]);
				Translation_apply(t);
				Communication_log(0,"length: %d, angle: %d", t.length, t.angle);
			}
			if (packetBuffer[0] == 23)
			{
				Waypoint wp1, wp2, wp3, wp4;
				wp1.x = 1000;
				wp1.y = 0;
				wp2.x = 1000;
				wp2.y = 1000;
				wp3.x = 0;
				wp3.y = 1000;
				wp4.x = wp4.y = 0;
				ObstacleAvoidance_addWaypoint(&wp1);
				ObstacleAvoidance_addWaypoint(&wp2);
				ObstacleAvoidance_addWaypoint(&wp3);
				ObstacleAvoidance_addWaypoint(&wp4);
			}
		}
		Task_waitCurrent(222);
	} while (1);
}
