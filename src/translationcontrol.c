#include <stdint.h>
#include "kernel/task.h"
#include "sensor/incremental.h"
#include "util/property.h"
#include "speed.h"
#include "device/motor.h"
#include "communication.h"


struct __TranslationControlStatus_t
{
	ControlStatus left;
	ControlStatus right;
} status;


WheelSpeed getCurrentSpeed(time_t timeSinceLast)
{
	WheelSpeed speed;
	WheelDistance distance = Incremental_getDistance();
	speed.left = (distance.left * 1000) / timeSinceLast;
	speed.right = (distance.right * 1000) / timeSinceLast;
	return speed;
}


static volatile WheelSpeed targetSpeed;


TASK_STATIC(speedTask,1,speedControl,250,0);

static void speedControl(void)
{
	time_t deltaTime = 1000;
	ControlParameters parameter;
	parameter.p = 4;
	parameter.i = 0;
	parameter.d = 0;
	status.left.integral = 0;
	status.left.lastValue = 0;
	status.right.integral = 0;
	status.right.lastValue = 0;
	do
	{
		WheelSpeed current;
		WheelSpeed target;
		WheelSpeed delta;
		// Get current speed
		current = getCurrentSpeed(deltaTime);
		// Read in wanted speed
		Property_copy(
				(uint8_t*)&target, (uint8_t*)&targetSpeed, sizeof(WheelSpeed));
		// Calculate error
		delta.left = target.left - current.left;
		delta.right = target.right - current.right;
		// Update integral values
		status.left.integral -= (delta.left * deltaTime) / 1000;
		status.right.integral -= (delta.right * deltaTime) / 1000;
		int16_t left = parameter.p * delta.left +
			parameter.i * (status.left.integral / 1000) +
			parameter.d * 
			((delta.left - status.left.lastValue) * 1000) / deltaTime;
		int16_t right = parameter.p * delta.right +
			parameter.i * (status.right.integral / 1000) +
			parameter.d * ((delta.right - status.right.lastValue) * 1000) / deltaTime;
		Communication_log(2,"L| c: %d |R| c: %d",
				current.left, current.right);
		// Set motors
		Motor_set4(250);
		Motor_set5(250);
		// Wait some time
		Task_waitCurrent(deltaTime);
	} while (1);
}



void Speed_setDesired(WheelSpeed * target)
{
	Property_copy((uint8_t*)&targetSpeed,
			(uint8_t*)target, sizeof(WheelSpeed));
}

