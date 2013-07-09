#include <stdint.h>
#include "util/ringbuffer.h"
#include "kernel/semaphore.h"
#include "kernel/pipe.h"




//-----------------------------------------------------------------------------
void Pipe_init(Pipe * pipe, uint8_t * buffer, uint8_t size)
{
	Ringbuffer_init(&(pipe->buffer),buffer,size);
	Semaphore_init(&(pipe->emptyCount),size);
	Semaphore_init(&(pipe->fillCount),0);
	Semaphore_init(&(pipe->writeLock),1);
	Semaphore_init(&(pipe->readLock),1);
}




//-----------------------------------------------------------------------------
void Pipe_startRead(Pipe * pipe)
{
	Semaphore_wait(&(pipe->readLock));
}




void Pipe_endRead(Pipe * pipe)
{
	Semaphore_signal(&(pipe->readLock));
}




void Pipe_startWrite(Pipe * pipe)
{
	Semaphore_wait(&(pipe->writeLock));
}




void Pipe_endWrite(Pipe * pipe)
{
	Semaphore_signal(&(pipe->writeLock));
}




//-----------------------------------------------------------------------------
void Pipe_write(Pipe * pipe, uint8_t data)
{
	Semaphore_wait(&(pipe->emptyCount));
	Ringbuffer_put(&(pipe->buffer),data);
	Semaphore_signal(&(pipe->fillCount));
}




uint8_t Pipe_tryWrite(Pipe * pipe, uint8_t data)
{
	if (Semaphore_try(&(pipe->emptyCount)))
	{
		Ringbuffer_put(&(pipe->buffer),data);
		Semaphore_signal(&(pipe->fillCount));
		return 1;
	}
	return 0;
}




void Pipe_read(Pipe * pipe, uint8_t * data)
{
	Semaphore_wait(&(pipe->fillCount));
	*data = Ringbuffer_get(&(pipe->buffer));
	Semaphore_signal(&(pipe->emptyCount));
}




uint8_t Pipe_tryRead(Pipe * pipe, uint8_t * data)
{
	if (Semaphore_try(&(pipe->fillCount)))
	{
		*data = Ringbuffer_get(&(pipe->buffer));
		Semaphore_signal(&(pipe->emptyCount));
		return 1;
	}
	return 0;
}

