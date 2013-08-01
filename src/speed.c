#include <stdint.h>
#include "device/motor.h"
#include "speed.h"



#define MAX_LENGTH 30
#define MOTOR_MAX 250.0
#define MOTOR_MIN 180.0
#define MOTOR_RANGE (MOTOR_MAX - MOTOR_MIN)


#define ROT_MIN 1.0
#define ROT_MAX 20.0
#define ROT_START 30.0

#define TRANS_MIN 0.0
#define TRANS_MAX MOTOR_RANGE


#define TRANS_SLOPE ((TRANS_MIN - TRANS_MAX)/(ROT_START))
#define ROT_SLOPE ((ROT_MAX - ROT_MIN)/(180 - ROT_START))

void Translation_apply(Translation t)
{
	int16_t left;
	int16_t right;
	t.angle += 270;
	if (t.angle > 360)
	{
		t.angle -= 360;
	}


	if (
			t.angle > (270 - ROT_START) &&
			t.angle < (270 + ROT_START))
	{
		if (t.angle < 270)
		{
			// left
			left = (270 - t.angle) * TRANS_SLOPE + TRANS_MAX;
			right = TRANS_MAX;
		}
		else
		{
			// right
			left = TRANS_MAX;
			right = (t.angle - 270) * TRANS_SLOPE + TRANS_MAX;
		}
		if (t.length < MAX_LENGTH)
		{
			left = (left * t.length) / MAX_LENGTH;
			right = (right * t.length) / MAX_LENGTH;
		}
	}
	else
	{
		int16_t aim = t.angle - 270;
		if (aim < 0)
		{
			aim += 360;
		}
		if (aim > 180)
		{
			int16_t val = (360 - aim) * ROT_SLOPE + ROT_MIN - ROT_START * ROT_SLOPE;
			left = -val;
			right = val;
		}
		else
		{
			int16_t val = (aim) * ROT_SLOPE + ROT_MIN - ROT_START * ROT_SLOPE;
			left = val;
			right = -val;
		}
	}
	// Scale to usable motor interval
	if (left < 0)
	{
		left -= MOTOR_MIN;
	}
	else if (left > 0)
	{
		left += MOTOR_MIN;
	}
	if (right < 0)
	{
		right -= MOTOR_MIN;
	}
	else if (right > 0)
	{
		right += MOTOR_MIN;
	}
	// Apply
	Motor_setLeft(left);
	Motor_setRight(right);
}

