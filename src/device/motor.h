#ifndef __DEVICE__MOTOR_H__
#define __DEVICE__MOTOR_H__ 1
#include <stdint.h>




//-----------------------------------------------------------------------------
/** Sets velocity of engine at timer 4. Range -255 to 255.
 * */
extern void Motor_setLeft(int16_t);




/** Sets velocity of engine at timer 5. Range -255 to 255.
 * */
extern void Motor_setRight(int16_t);




//-----------------------------------------------------------------------------
#endif

