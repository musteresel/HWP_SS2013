#include <stdint.h>
#include "kernel/task.h"
#include "util/onewriter.h"

void Onewriter_init(Onewriter * w, void * data, uint8_t size)
{
	w->valid = 0;
	w->count = 0;
	w->data = (uint8_t *) data;
	w->size = size;
}


void Onewriter_write(Onewriter * w, void * source)
{
	uint8_t * src = (uint8_t *) source;
	volatile uint8_t * dest = w->data;
	uint8_t iterator;
	w->valid = 0;
	asm volatile (""::: "memory");
	for (iterator = 0; iterator < w->size; iterator++)
	{
		*dest = *src;
		src++;
		dest++;
	}
	asm volatile (""::: "memory");
	w->count++;
	asm volatile (""::: "memory");
	w->valid = 0xFF;
}

void Onewriter_read(Onewriter * w, void * destination)
{
	volatile uint8_t * src = (uint8_t *) w->data;
	uint8_t * dest = (uint8_t *) destination;
	uint8_t iterator;
	uint8_t count;
	while (!w->valid)
	{
		Task_waitCurrent(20);
	}
	asm volatile ("" ::: "memory");
	do
	{
		asm volatile ("" ::: "memory");
		count = w->count;
		asm volatile ("" ::: "memory");
		for (iterator = 0; iterator < w->size; iterator++)
		{
			*dest = *src;
			src++;
			dest++;
		}
		asm volatile ("" ::: "memory");
	} while (w->count != count);
}

