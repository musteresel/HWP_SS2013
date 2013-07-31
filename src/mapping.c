#include <stdint.h>
#include <math.h>
#include "kernel/task.h"
#include <avr/interrupt.h>
#include "sensor/incremental.h"
#include "communication.h"
#include "util/w1r1.h"
#include "mapping.h"



//-----------------------------------------------------------------------------
#define WIDTH 108
#define LENGTH_TO_MID 50
#define PI 3.14159265359d



Pose _robotPose;
W1R1 robotPose;



void updatePose(WheelDistance distance)
{
	Pose current = _robotPose;
	double sum = (distance.left + distance.right) / 2;
	double diff = distance.left - distance.right;
	double dTheta = diff / WIDTH;
	current.theta += dTheta;
	if (current.theta < 0)
	{
		current.theta += 2 * PI;
	}
	else if (current.theta > 2 * PI)
	{
		current.theta -= 2 * PI;
	}
	double sinTheta = sin(current.theta);
	double cosTheta = cos(current.theta);
  double dx = (sum * cosTheta) - (LENGTH_TO_MID * sinTheta)*dTheta;
	double dy = (sum * sinTheta) + (LENGTH_TO_MID * cosTheta)*dTheta;
	/*dx >>= 1;
	dy >>= 1;*/
	current.x += dx;
	current.y += dy;
	W1R1_write(&robotPose,&current);
}



TASK_STATIC(mapping,1,mappingFct,190,1);

static void mappingFct(void)
{
	_robotPose.x = 0;
	_robotPose.y = 0;
	_robotPose.theta = 0;
	W1R1_init(&robotPose, &_robotPose, sizeof(Pose));
	do
	{
		// Lese inc aus
		WheelDistance distance = Incremental_getDistance();
		// Berechne neue Pose
		updatePose(distance);
		// Lese ir aus && Berechne Wände
		// updateWalls();
		// (TODO)evtl: Korrigiere Pose
		// Warte 200ms
		Task_waitCurrent(200);
	} while (1);
}

TASK_STATIC(mappingSend,4,send,200,1);


struct PosePacket
{
	int32_t x;
	int32_t y;
	int16_t theta;
};

static void send(void)
{
	Task_waitCurrent(500);
	do
	{
		struct PosePacket current;
		Pose pose;
		W1R1_read(&robotPose,&pose);
		current.x = pose.x;
		current.y = pose.y;
		current.theta = (pose.theta * 180) / PI;
		current.theta -= 270;
		if (current.theta < 0)
		{
			current.theta += 360;
		}
		Communication_writePacket(2,(uint8_t*)&current,sizeof(struct PosePacket));
		Task_waitCurrent(2000);
	} while(1);
}

