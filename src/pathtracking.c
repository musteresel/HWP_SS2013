#include <stdint.h>
#include "kernel/task.h"
#include "kernel/semaphore.h"
#include "util/attribute.h"
#include "util/ringbuffer.h"
#include "util/w1r1.h"
#include <math.h>
#include "mapping.h"
#include "speed.h"
#include "pathtracking.h"




#define MAX_WAYPOINTS 4
#define THRESHOLD 30

//uint8_t wayPointFlag = 1;	// Damit der Roboter ueberhaupt losfaehrt
SEMAPHORE_STATIC(waypointFlag,0);

uint8_t __waypointbuffer[MAX_WAYPOINTS * sizeof(Waypoint)];
Ringbuffer waypointBuffer;
SEMAPHORE_STATIC(fillCount, 0);
SEMAPHORE_STATIC(emptyCount,MAX_WAYPOINTS);
ATTRIBUTE( constructor, used ) static void pathtracking_ctor(void)
{
	Ringbuffer_init(&waypointBuffer,__waypointbuffer,MAX_WAYPOINTS * sizeof(Waypoint));
}




void Pathtracking_addWaypoint(Waypoint * wp)
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




TASK_STATIC(path,3,pathFct,200,1);
#define PI 3.14159265359d

static void pathFct(void)
{
	Waypoint waypoint;
	Pose pose;
	Translation translation;
	do
	{
		 getWaypoint(&waypoint);
		 do
		 {
			 W1R1_read(&robotPose,&pose);
			 // Vektor berechnen
			 double dx = waypoint.x - pose.x;
			 double dy = waypoint.y - pose.y;
			 //Communication_log(0,"(%f | %f)",dx,dy);
			 double sum;
			 if (dx < 0)
			 {
				 sum = -dx;
			 }
			 else
			 {
				 sum = dx;
			 }
			 if (dy < 0)
			 {
				 sum += - dy;
			 }
			 else
			 {
				 sum += dy;
			 }
			 if (sum < THRESHOLD)
			 {
				 translation.length = 0;
				 Translation_apply(translation);
				 //wayPointFlag++;
				 Semaphore_signal(&waypointFlag);
				 break;
			 }
			 else if (sum > 1000)
			 {
				 translation.length = 1000;
			 }
			 else
			 {
				 translation.length = sum;
			 }
			 // TODO above: better scaling of t.lenght (is signed now)
			 double targetAngle = atan2(dy,dx);
			 if (targetAngle < 0)
			 {
				 targetAngle += 2 * PI;
			 }
			 double aimAngle = targetAngle - pose.theta;
			 if (aimAngle < 0)
			 {
				 aimAngle += 2 * PI;
			 }
			 translation.angle = (aimAngle * 180) / PI;
			 //Communication_log(0,"Aim: %d",translation.angle);
			 Translation_apply(translation);
			 Task_waitCurrent(500);
		 } while (1);
	} while (1);
}


