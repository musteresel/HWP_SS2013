#include <stdint.h>
#include "util/ringbuffer.h"
#include "kernel/semaphore.h"
#include "kernel/pipe.h"


void Pipe11_init(Pipe11 * pipe, uint8_t * buffer, uint8_t size)
{
	Ringbuffer_init(&(pipe->buffer),buffer,size);
	Semaphore_init(&(pipe->emptyCount),size);
	Semaphore_init(&(pipe->fillCount),0);
}
void Pipe11_write(Pipe11 * pipe, uint8_t data)
{
	Semaphore_wait(&(pipe->emptyCount));
	Ringbuffer_put(&(pipe->buffer), data);
	Semaphore_signal(&(pipe->fillCount));
}
uint8_t Pipe11_read(Pipe11 * pipe)
{
	Semaphore_wait(&(pipe->fillCount));
	uint8_t data = Ringbuffer_get(&(pipe->buffer));
	Semaphore_signal(&(pipe->emptyCount));
	return data;
}



void Pipe1N_init(Pipe1N * pipe, uint8_t * buffer, uint8_t size)
{
	Ringbuffer_init(&(pipe->buffer),buffer,size);
	Semaphore_init(&(pipe->emptyCount),size);
	Semaphore_init(&(pipe->fillCount),0);
	Semaphore_init(&(pipe->readLock),1);
}
void Pipe1N_write(Pipe1N * pipe, uint8_t data)
{
	Semaphore_wait(&(pipe->emptyCount));
	Ringbuffer_put(&(pipe->buffer), data);
	Semaphore_signal(&(pipe->fillCount));
}
uint8_t Pipe1N_read(Pipe1N * pipe)
{
	Semaphore_wait(&(pipe->fillCount));
	Semaphore_wait(&(pipe->readLock));
	uint8_t data = Ringbuffer_get(&(pipe->buffer));
	Semaphore_signal(&(pipe->readLock));
	Semaphore_signal(&(pipe->emptyCount));
	return data;
}



void PipeN1_init(PipeN1 * pipe, uint8_t * buffer, uint8_t size)
{
	Ringbuffer_init(&(pipe->buffer),buffer,size);
	Semaphore_init(&(pipe->emptyCount),size);
	Semaphore_init(&(pipe->fillCount),0);
	Semaphore_init(&(pipe->writeLock),1);
}
void PipeN1_write(PipeN1 * pipe, uint8_t data)
{
	Semaphore_wait(&(pipe->emptyCount));
	Semaphore_wait(&(pipe->writeLock));
	Ringbuffer_put(&(pipe->buffer), data);
	Semaphore_signal(&(pipe->writeLock));
	Semaphore_signal(&(pipe->fillCount));
}
uint8_t PipeN1_read(PipeN1 * pipe)
{
	Semaphore_wait(&(pipe->fillCount));
	uint8_t data = Ringbuffer_get(&(pipe->buffer));
	Semaphore_signal(&(pipe->emptyCount));
	return data;
}




void PipeNN_init(PipeNN * pipe, uint8_t * buffer, uint8_t size)
{
	Ringbuffer_init(&(pipe->buffer),buffer,size);
	Semaphore_init(&(pipe->emptyCount),size);
	Semaphore_init(&(pipe->fillCount),0);
	Semaphore_init(&(pipe->writeLock),1);
	Semaphore_init(&(pipe->readLock),1);
}
void PipeNN_write(PipeNN * pipe, uint8_t data)
{
	Semaphore_wait(&(pipe->emptyCount));
	Semaphore_wait(&(pipe->writeLock));
	Ringbuffer_put(&(pipe->buffer), data);
	Semaphore_signal(&(pipe->writeLock));
	Semaphore_signal(&(pipe->fillCount));
}
uint8_t PipeNN_read(PipeNN * pipe)
{
	Semaphore_wait(&(pipe->fillCount));
	Semaphore_wait(&(pipe->readLock));
	uint8_t data = Ringbuffer_get(&(pipe->buffer));
	Semaphore_signal(&(pipe->readLock));
	Semaphore_signal(&(pipe->emptyCount));
	return data;
}




