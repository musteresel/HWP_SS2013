#ifndef __MAPPING_H__
#define __MAPPING_H__ 1
#include <stdint.h>
#include "util/w1r1.h"

typedef struct __Pose_t
{
	double x;
	double y;
	double theta; // Rad (0 - 2PI)
} Pose;


extern W1R1 robotPose;

#endif

