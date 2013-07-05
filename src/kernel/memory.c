#include <stdlib.h>
#include <stdint.h>
#include "kernel/semaphore.h"




//-----------------------------------------------------------------------------
static Semaphore memLock;




//-----------------------------------------------------------------------------
uint8_t * Memory_alloc(uint8_t size)
{
	Semaphore_wait(&memLock);
	uint8_t * memory = (uint8_t *) malloc(size);
	Semaphore_signal(&memLock);
	return memory;
}




void Memory_free(uint8_t * memory)
{
	Semaphore_wait(&memLock);
	free(memory);
	Semaphore_signal(&memLock);
}




void Memory_init(void)
{
	Semaphore_init(&memLock,1);
}

