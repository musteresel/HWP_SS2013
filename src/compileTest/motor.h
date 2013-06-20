#ifndef MOTOR_H_
#define MOTOR_H_

#include <global.h>


/**
 * Initialize the Motor Driver
 */
void Motor_init();

/**
 * int16_t velocity	a value in [-255...+255] setting direction and speed
 */
void Motor_setVelocity_MotorA(int16_t velocity);

/**
 * int16_t velocity	a value in [-255...+255] setting direction and speed
 */
void Motor_setVelocity_MotorB(int16_t velocity);


#endif /*MOTOR_H_*/
