#ifndef __SPEED_H__
#define __SPEED_H__ 1
#include <stdint.h>



typedef struct __Translation_t
{
	int16_t speed;
	int16_t steering;
} Translation;


extern void Translation_set(Translation *);
#endif

