#ifndef __SENSOR_IR_H__
#define __SENSOR_IR_H__ 1

#include <stdint.h>

#define IR_FROM_CM(c) ((c) << 1)
#define IR_TO_CM(l) ((l) >> 1)

#define IR_FRONT 0
#define IR_LEFT 2
#define IR_RIGHT 1

typedef uint8_t dist_t;

extern dist_t Ir_read(uint8_t pos);

#endif
