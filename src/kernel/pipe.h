/** Implementation of a simple pipe type using kernel semaphores and
 * ringbuffers.
 *
 * Currently, no select operation is available!
 * */
#ifndef __KERNEL__PIPE_H__
#define __KERNEL__PIPE_H__ 1
#include <stdint.h>
#include "util/ringbuffer.h"
#include "util/attribute.h"
#include "kernel/semaphore.h"




//-----------------------------------------------------------------------------
/** Macro to create a static available pipe.
 * */
#define PIPE_STATIC(name, size) \
	NOINIT Pipe name; \
	NOINIT uint8_t name ## Buffer[size]; \
	ATTRIBUTE( constructor, used ) \
	static void Pipe_ctor_ ## name (void) \
	{ \
		Pipe_init(&name,name ## Buffer,size); \
	}




//-----------------------------------------------------------------------------
/** Type definition of a pipe.
 * */
typedef struct __Pipe_t
{
	Ringbuffer buffer;
	Semaphore emptyCount;
	Semaphore fillCount;
	Semaphore readLock;
	Semaphore writeLock;
} Pipe;




//-----------------------------------------------------------------------------
/** Initialize a pipe control structure with a given buffer and its size.
 * */
extern void Pipe_init(Pipe *, uint8_t *, uint8_t);




//-----------------------------------------------------------------------------
/** Set read lock of a pipe. Blocking!
 * */
extern void Pipe_startRead(Pipe *);




/** Clear read lock of a pipe.
 * */
extern void Pipe_endRead(Pipe *);




/** Set write lock of a pipe. Blocking!
 * */
extern void Pipe_startWrite(Pipe *);




/** Clear write lock of a pipe.
 * */
extern void Pipe_endWrite(Pipe *);




//-----------------------------------------------------------------------------
/** Write to a pipe. One must ensure that no other task is writing to that
 * pipe, using Pipe_startWrite and Pipe_endWrite. Blocking!
 * */
extern void Pipe_write(Pipe *, uint8_t);




/** Try to write to a pipe. Same as for Pipe_write applies, except that this is
 * nonblocking.
 *
 * Returns 1 on success, 0 otherwise.
 * */
extern uint8_t Pipe_tryWrite(Pipe *, uint8_t);




/** Read from a pipe. One must ensure that no other task is reading from that
 * pipe, using Pipe_startRead and Pipe_endRead. Blocking!
 *
 * The datum read is returned by reference!
 * */
extern void Pipe_read(Pipe *, uint8_t *);




/** Try to read from a pipe. Same as for Pipe_read applies, except that this is
 * nonblocking.
 *
 * Returns 1 on success, 0 otherwise. Read datum is returned by reference!
 * */
extern uint8_t Pipe_tryRead(Pipe *, uint8_t *);




//-----------------------------------------------------------------------------
#endif

