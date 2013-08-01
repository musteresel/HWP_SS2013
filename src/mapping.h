#ifndef __MAPPING_H__
#define __MAPPING_H__ 1
#include <stdint.h>
#include "util/onewriter.h"

typedef struct __Pose_t
{
	double x;
	double y;
	double theta; // Rad (0 - 2PI)
} Pose;


extern Onewriter robotPose;

#endif

