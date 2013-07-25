#ifndef __UTIL__W1R1_H__
#define __UTIL__W1R1_H__ 1
#include <stdint.h>



typedef struct __w1r1_t
{
	volatile uint8_t valid;
	volatile uint8_t read;
	uint8_t * data;
	uint8_t size;
} W1R1;

/*
//void W1R1_startWrite(volatile W1R1 *);
#define W1R1_startWrite(w1r1) \
	(w1r1)->valid = 0; (w1r1)->read = 0;
//void W1R1_endWrite(volatile W1R1 *);
#define W1R1_endWrite(w1r1) \
	(w1r1)->valid = 0xFF;
//void W1R1_startRead(volatile W1R1 *);
#define W1R1_startRead(w1r1) \
	while (!(w1r1)->valid) Task_yield(); \
	(w1r1)->read = 0xFF;
//uint8_t W1R1_endRead(volatile W1R1 *);
#define W1R1_endRead(w1r1) \
	(w1r1).read == 0xFF
void W1R1_init(volatile W1R1 *,void *);

void * W1R1_read(volatile W1R1 * w1r1);
uint8_t W1R1_check(volatile W1R1 * w1r1);

*/

void W1R1_init(W1R1 *, void *, uint8_t);
void W1R1_write(W1R1 *, void *);
void W1R1_read(W1R1 *, void *);



#endif

