#ifndef __PATHTRACKING_H__
#define __PATHTRACKING_H__ 1
#include <stdint.h>
#include "kernel/semaphore.h"

typedef struct __Waypoint_t
{
	int16_t x;
	int16_t y;
} Waypoint;

//uint8_t wayPointFlag;	// Wird hochgezaehlt, sobald ein Wegpunkt erreicht wurde
extern Semaphore waypointFlag;

void Pathtracking_addWaypoint(Waypoint *);


#endif

