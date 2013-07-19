#ifndef __SPEED_H__
#define __SPEED_H__ 1
#include <stdint.h>



typedef struct __Translation_t
{
	uint16_t length;
	int16_t angle;
} Translation;


extern void Translation_apply(Translation);
#endif

