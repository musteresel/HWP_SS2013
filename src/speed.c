#include <stdint.h>
#include "device/motor.h"
#include "speed.h"



#define INNER_ZERO 200
#define INNER_MIN 135
#define OUTER_ZERO 45
#define MAX_LENGTH 200
#define MOTOR_MAX 250
#define MOTOR_MIN 180
#define MOTOR_RANGE (MOTOR_MAX - MOTOR_MIN)




void Translation_apply(Translation t)
{
	int16_t left;
	int16_t right;
	t.angle += 270;
	if (t.angle > 360)
	{
		t.angle -= 360;
	}


	if (t.angle > 260 && t.angle < 280)
	{
		left = MOTOR_RANGE;
		right = MOTOR_RANGE;
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
			int16_t val = (10 - ((aim - 180) * (10-1))/180);
			left = -val;
			right = val;
		}
		else
		{
			int16_t val = (1 + ((aim) * (10-1))/180);
			left = val;
			right = -val;
		}
	}

/*	
	if (t.angle > 270)
	{
		left = MOTOR_RANGE;
	}
	else if (t.angle > INNER_ZERO)
	{
		left = (MOTOR_RANGE *(t.angle - INNER_ZERO)) / (270-INNER_ZERO);
	}
	else if (t.angle > INNER_MIN)
	{
		left = -MOTOR_RANGE +
			(MOTOR_RANGE * (t.angle - INNER_MIN)) / (INNER_ZERO - INNER_MIN);
	}
	else if (t.angle > 90)
	{
		left = -MOTOR_RANGE;
	}
	else if (t.angle > OUTER_ZERO)
	{
		left = (-(MOTOR_RANGE-10) * (t.angle - OUTER_ZERO)) / (90 - OUTER_ZERO);
	}
	else
	{
		left = (MOTOR_RANGE * (OUTER_ZERO - t.angle)) / OUTER_ZERO;
	}
	t.angle = 180 - t.angle;
	if (t.angle < 0)
	{
		t.angle += 360;
	}
	if (t.angle > 270)
	{
		right = MOTOR_RANGE;
	}
	else if (t.angle > INNER_ZERO)
	{
		right = (MOTOR_RANGE *(t.angle - INNER_ZERO)) / (270-INNER_ZERO);
	}
	else if (t.angle > INNER_MIN)
	{
		right = -MOTOR_RANGE +
			(MOTOR_RANGE * (t.angle - INNER_MIN)) / (INNER_ZERO - INNER_MIN);
	}
	else if (t.angle > 90)
	{
		right = -MOTOR_RANGE;
	}
	else if (t.angle > OUTER_ZERO)
	{
		right = (-(MOTOR_RANGE-10) * (t.angle - OUTER_ZERO)) / (90 - OUTER_ZERO);
	}
	else
	{
		right = (MOTOR_RANGE * (OUTER_ZERO - t.angle)) / OUTER_ZERO;
	}
	*/
	
	/*if (t.length < 10)
	{
		left = 0;
		right = 0;
	}*/
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
	Motor_setLeft(left);
	Motor_setRight(right);
}

/*



void Translation_apply(Translation t)
{
*/
