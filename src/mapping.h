#ifndef __MAPPING_H__
#define __MAPPING_H__ 1
#include <stdint.h>
#include "util/w1r1.h"


// Karte
#define MAP_SIZE 7		// Kantenlaenge
#define CELL_SIZE 250	// Kantenlaenge in mm


typedef struct __Pose_t
{
	double x;
	double y;
	double theta; // Rad (0 - 2PI)
} Pose;

typedef struct __Cell_t
{
	uint8_t directions;
	int8_t pot;
} Cell;



extern W1R1 robotPose;


#endif

