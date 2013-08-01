#include <stdint.h>
#include <math.h>
#include "kernel/task.h"
#include "kernel/semaphore.h"
#include "util/ringbuffer.h"
#include "sensor/ir.h"
#include "pathtracking.h"
#include "mapping.h"


#define LIMIT 300
#define STEP_ASIDE 250
#define TURN_ANGLE (M_PI/2)


#define MAX_WAYPOINTS 8

uint8_t __waypointbuffer[MAX_WAYPOINTS * sizeof(Waypoint)];
Ringbuffer waypointBuffer;
SEMAPHORE_STATIC(fillCount, 0);
SEMAPHORE_STATIC(emptyCount,MAX_WAYPOINTS);
ATTRIBUTE( constructor, used ) static void obstacleavoidance_ctor(void)
{
	Ringbuffer_init(&waypointBuffer,__waypointbuffer,MAX_WAYPOINTS * sizeof(Waypoint));
}




void ObstacleAvoidance_addWaypoint(Waypoint * wp)
{
	Semaphore_wait(&emptyCount);
	uint8_t * wp_pt = (uint8_t *) wp;
	uint8_t iterator;
	for (iterator = 0; iterator < sizeof(Waypoint); iterator++)
	{
		Ringbuffer_put(&waypointBuffer, *wp_pt);
		wp_pt++;
	}
	Semaphore_signal(&fillCount);
}

static void getWaypoint(Waypoint * wp)
{
	Semaphore_wait(&fillCount);
	uint8_t * wp_pt = (uint8_t *) wp;
	uint8_t iterator;
	for (iterator = 0; iterator < sizeof(Waypoint); iterator++)
	{
		*wp_pt = Ringbuffer_get(&waypointBuffer);
		wp_pt++;
	}
	Semaphore_signal(&emptyCount);
}




TASK_STATIC(obstacleAvoidance,3,avoid,200,1);
static void avoid(void)
{
	Waypoint waypoint;
	Waypoint immediateWaypoint;
	uint8_t reachedWaypoint = 0;
	do
	{
		// Read next planned waypoint
		getWaypoint(&waypoint);
		reachedWaypoint = 0;
		do
		{
			// Read robot position
			Pose position;
			Onewriter_read(&robotPose,&position);
			double dx = waypoint.x - position.x;
			double dy = waypoint.y - position.y;
			double distanceToWaypoint = sqrt(dx * dx + dy * dy);
			double aimAngle = atan2(dy,dx);
			if (aimAngle < 0)
			{
				aimAngle += 2 * M_PI;
			}
			double deltaAlpha = aimAngle - position.theta;
			if (deltaAlpha < 0)
			{
				deltaAlpha += 2 * M_PI;
			}
			if (deltaAlpha > M_PI)
			{
				deltaAlpha = 2 * M_PI - deltaAlpha;
			}
			// Read ir sensor
			uint16_t irDistance = Ir_read(IR_FRONT) * 20;
			if (irDistance < LIMIT)
			{
				immediateWaypoint.x = position.x + STEP_ASIDE * cos(aimAngle + TURN_ANGLE);
				immediateWaypoint.y = position.y + STEP_ASIDE * sin(aimAngle + TURN_ANGLE);
			}
			else
			{
				uint8_t distanceToNext = 50 + (1 - deltaAlpha/M_PI) * (LIMIT/3 - 50);
				if (distanceToWaypoint < distanceToNext)
				{
					distanceToNext = distanceToWaypoint;
					reachedWaypoint = 1;
				}
				immediateWaypoint.x = position.x + distanceToNext * cos(aimAngle);
				immediateWaypoint.y = position.y + distanceToNext * sin(aimAngle);
			}
			Pathtracking_addWaypoint(&immediateWaypoint);
		} while (reachedWaypoint == 0);	
	} while (1);
}




