#include <stdint.h>
#include <math.h>
#include "kernel/task.h"
#include <avr/interrupt.h>
#include "sensor/incremental.h"
#include "communication.h"

#define WIDTH 115
#define PI 3.14159265359

typedef struct __Pose_t
{
	int16_t x;
	int16_t y;
	double theta; // Rad (0 - 2PI)
} Pose;


volatile Pose currentPose;


void updatePose(WheelDistance distance)
{
	int16_t sum = distance.left + distance.right;
	int16_t diff = distance.left - distance.right;
	double dTheta = (double) diff / (double) WIDTH;
	int16_t dx = (sum * cos(currentPose.theta));
	int16_t dy = (sum * sin(currentPose.theta));
	dx >>= 1;
	dy >>= 1;
	dTheta = currentPose.theta + dTheta;
	if (dTheta < 0)
	{
		dTheta += 2 * PI;
	}
	else if (dTheta > 2 * PI)
	{
		dTheta -= 2 * PI;
	}
	// TODO wait/lockfree code
	currentPose.theta = dTheta;
	currentPose.x += dx;
	currentPose.y += dy;
	// TODO wait/lockfree code
}



TASK_STATIC(mapping,1,mappingFct,190,1);

static void mappingFct(void)
{
	currentPose.x = 0;
	currentPose.y = 0;
	currentPose.theta = 0;
	uint8_t i = 0;
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
		if (i == 10)
		{
			Communication_log(0,"x:%d y:%d theta:%f",currentPose.x,currentPose.y,
					currentPose.theta);
			i = 0;
		}
		i++;
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
		cli();
		current.x = currentPose.x;
		current.y = currentPose.y;
		current.theta = (currentPose.theta * 180) / PI;
		sei();
		current.theta -= 270;
		if (current.theta < 0)
		{
			current.theta += 360;
		}
		Communication_writePacket(2,(uint8_t*)&current,sizeof(struct PosePacket));
		Task_waitCurrent(1000);
	} while(1);
}

