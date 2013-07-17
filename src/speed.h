#ifndef __SPEED_H__
#define __SPEED_H__ 1
#include <stdint.h>




typedef struct __WheelSpeed_t
{
	int16_t left;
	int16_t right;
} WheelSpeed;


typedef struct __ControlParameters_t
{
	int8_t p;
	int8_t i;
	int8_t d;
} ControlParameters;

typedef struct __ControlStatus_t
{
	int16_t integral;
	int16_t lastValue;
} ControlStatus;

extern void Speed_setDesired(WheelSpeed *);


#endif

