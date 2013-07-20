#include <stdint.h>
#include <math.h>
#include "kernel/task.h"
#include "sensor/incremental.h"
#include "communication.h"

#define WIDTH 115
#define PI 3.14159265359

typedef struct __Pose_t
{
	int16_t x;
	int16_t y;
	int16_t theta; // Grad (0 - 360)
} Pose;


volatile Pose currentPose;


void calculateNewPose(WheelDistance distance)
{
	currentPose.theta += ((distance.left - distance.right) * 180) / (PI * WIDTH);
	int16_t sum = distance.left + distance.right;
	currentPose.x += (sum * sin((double)currentPose.theta / 180 * PI))
		/ (currentPose.theta << 1);
	currentPose.y += (sum * (1 - cos((double)currentPose.theta / 180 * PI)))
		/ (currentPose.theta << 1);
}


TASK_STATIC(mapping,1,mappingFct,190,1);

static void mappingFct(void)
{
	currentPose.x = 0;
	currentPose.y = 0;
	currentPose.theta = 0;
	do
	{
		// Lese inc aus
		WheelDistance distance = Incremental_getDistance();
		// Berechne neue Pose
		calculateNewPose(distance);
		// Lese ir aus && Berechne Wände
		// updateWalls();
		// (TODO)evtl: Korrigiere Pose
		// Warte 100ms
		Communication_log(0,"L:%d R:%d",distance.left, distance.right);
		Task_waitCurrent(500);
	} while (1);
}

TASK_STATIC(mappingSend,4,send,200,0);


struct PosePacket
{
	int64_t x;
	int64_t y;
	int16_t theta;
};

static void send(void)
{
	Task_waitCurrent(500);
	do
	{
		struct PosePacket current;
		current.x = currentPose.x;
		current.y = currentPose.y;
		current.theta = currentPose.theta;
		Communication_writePacket(2,(uint8_t*)&current,sizeof(struct PosePacket));
		Task_waitCurrent(1000);
	} while(1);
}

