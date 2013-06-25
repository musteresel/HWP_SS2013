#ifndef __SENSOR_IR_H__
#define __SENSOR_IR_H__ 1

#include <stdint.h>

#define FROM_CM(c) ((c) << 1)
#define TO_CM(l) ((l) >> 1)

#define IR_FRONT 0
#define IR_LEFT 2
#define IR_RIGHT 1

typedef uint8_t dist_t;

extern dist_t readIR(uint8_t pos);

#endif
