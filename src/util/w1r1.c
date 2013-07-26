
#include "util/w1r1.h"

/*void W1R1_startWrite(volatile W1R1 * w1r1)
{
	w1r1->valid = 0;
	w1r1->read = 0;
}


void W1R1_endWrite(volatile W1R1 * w1r1)
{
	w1r1->valid = 0xFF;
}*/
/*
void W1R1_startRead(volatile W1R1 * w1r1)
{
	w1r1->read = 0xFF;
}

uint8_t W1R1_endRead(volatile W1R1 * w1r1)
{
	return (w1r1->read == 0xFF);
}
*/
/*
void W1R1_init(volatile W1R1 * w1r1, void * data)
{
	w1r1->valid = 0;
	w1r1->read = 0;
	w1r1->data = data;
}


void * W1R1_read(volatile W1R1 * w1r1)
{
	while (!w1r1->valid) Task_yield();
	w1r1->read = 0xFF;
	return w1r1->data;
}
uint8_t W1R1_check(volatile W1R1 * w1r1)
{
	return (w1r1->read == 0xFF);
}*/


void W1R1_init(W1R1 * w1r1, void * data, uint8_t size)
{
	w1r1->valid = 0;
	w1r1->read = 0;
	w1r1->data = (uint8_t *) data;
	w1r1->size = size;
}


void W1R1_write(W1R1 * w1r1, void * source)
{
	uint8_t * src = (uint8_t *) source;
	volatile uint8_t * dest = w1r1->data;
	uint8_t iterator;
	w1r1->valid = 0;
	w1r1->read = 0;
	asm volatile (""::: "memory");
	for (iterator = 0; iterator < w1r1->size; iterator++)
	{
		*dest = *src;
		src++;
		dest++;
	}
	asm volatile (""::: "memory");
	w1r1->valid = 0xFF;
}

void W1R1_read(W1R1 * w1r1, void * destination)
{
	volatile uint8_t * src = w1r1->data;
	uint8_t * dest = (uint8_t *) destination;
	uint8_t iterator;
	do
	{
		// TODO
		w1r1->read = 0xFF;
		asm volatile (""::: "memory");
		for (iterator = 0; iterator < w1r1->size; iterator++)
		{
			*dest = *src;
			src++;
			dest++;
		}
		asm volatile (""::: "memory");
	} while (w1r1->read != 0xFF);
}
	
