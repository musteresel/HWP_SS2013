#ifndef __PATHTRACKING_H__
#define __PATHTRACKING_H__ 1
#include <stdint.h>


typedef struct __Waypoint_t
{
	int16_t x;
	int16_t y;
} Waypoint;

void Pathtracking_addWaypoint(Waypoint *);


#endif

