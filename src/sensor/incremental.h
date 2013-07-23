#ifndef __SENSOR__INCREMENTAL_H__
#define __SENSOR__INCREMENTAL_H__ 1
#include <stdint.h>




//-----------------------------------------------------------------------------
/** Packed information about traveled distance in mm.
 * */
typedef struct __WheelDistance_t
{
	double left;
	double right;
} WheelDistance;




//-----------------------------------------------------------------------------
/** Returns traveled distance of both wheels since the last call.
 *
 * Should be called at least 1 time per second.
 * */
extern WheelDistance Incremental_getDistance(void);




//-----------------------------------------------------------------------------
#endif

