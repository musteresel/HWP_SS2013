#ifndef __IRSENSORS_H__
#define __IRSENSORS_H__ 1
#include <stdint.h>
#include <sensor/ir.h>




//-----------------------------------------------------------------------------
/** Struct to pack sensor data
 * */
typedef struct __IrSensorData_t
{
	dist_t front;
	dist_t left;
	dist_t right;
} IrSensorData;




//-----------------------------------------------------------------------------
/** Current sensor data.
 * */
extern IrSensorData irSensorData;




//-----------------------------------------------------------------------------
#endif

