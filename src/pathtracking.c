#include <stdint.h>
#include "kernel/task.h"
#include "kernel/semaphore.h"
#include "util/onewriter.h"
#include <math.h>
#include "mapping.h"
#include "speed.h"
#include "pathtracking.h"




#define THRESHOLD 30

SEMAPHORE_STATIC(waypointAvailable,0);
SEMAPHORE_STATIC(atWaypoint,0);

static Waypoint waypoint;

void Pathtracking_addWaypoint(Waypoint * wp)
{
	waypoint = *wp;
	Semaphore_signal(&waypointAvailable);
	Semaphore_wait(&atWaypoint);
}


TASK_STATIC(path,3,pathFct,200,1);
#define PI 3.14159265359d

static void pathFct(void)
{
	Pose pose;
	Translation translation;
	do
	{
		 //getWaypoint(&waypoint);
		 Semaphore_wait(&waypointAvailable);
		 do
		 {
			 Onewriter_read(&robotPose,&pose);
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
		 Semaphore_signal(&atWaypoint);
	} while (1);
}


