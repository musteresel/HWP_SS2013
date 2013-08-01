#ifndef __UTIL__ONEWRITER_H__
#define __UTIL__ONEWRITER_H__ 1
#include <stdint.h>


typedef struct __Onewriter_t
{
	volatile uint8_t count;
	volatile uint8_t valid;
	uint8_t * data;
	uint8_t size;
} Onewriter;



void Onewriter_init(Onewriter *, void *, uint8_t);
void Onewriter_write(Onewriter *, void *);
void Onewriter_read(Onewriter *, void *);

#endif

