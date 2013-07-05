/** Implementation of various pipe types using kernel semaphores and
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
#define PIPE_STATIC(name, type, size) \
	NOINIT Pipe ## type name; \
	NOINIT static uint8_t name ## Buffer[size]; \
	ATTRIBUTE( constructor, used ) \
	static void Pipe_ctor_ ## name (void) \
	{ \
		Pipe ## type ## _init(&name,name ## Buffer,size); \
	}




/** Shortcut to create a static 11 type pipe.
 * */
#define PIPE11_STATIC(name, size) PIPE_STATIC(name, 11, size)




/** Shortcut to create a static 1N type pipe.
 * */
#define PIPE1N_STATIC(name, size) PIPE_STATIC(name, 1N, size)




/** Shortcut to create a static N1 type pipe.
 * */
#define PIPEN1_STATIC(name, size) PIPE_STATIC(name, N1, size)




/** Shortcut to create a static NN type pipe.
 * */
#define PIPENN_STATIC(name, size) PIPE_STATIC(name, NN, size)




//-----------------------------------------------------------------------------
/** Type definition of a 11 pipe.
 * */
typedef struct __Pipe11_t
{
	Ringbuffer buffer;
	Semaphore emptyCount;
	Semaphore fillCount;
} Pipe11;




/** Type definition of a 1N pipe.
 * */
typedef struct __Pipe1N_t
{
	Ringbuffer buffer;
	Semaphore emptyCount;
	Semaphore fillCount;
	Semaphore readLock;
} Pipe1N;




/** Type definition of a N1 pipe.
 * */
typedef struct __PipeN1_t
{
	Ringbuffer buffer;
	Semaphore emptyCount;
	Semaphore fillCount;
	Semaphore writeLock;
} PipeN1;




/** Type definition of a NN pipe.
 * */
typedef struct __PipeNN_t
{
	Ringbuffer buffer;
	Semaphore emptyCount;
	Semaphore fillCount;
	Semaphore readLock;
	Semaphore writeLock;
} PipeNN;




//-----------------------------------------------------------------------------
extern void Pipe11_init(Pipe11 *, uint8_t *, uint8_t);
extern void Pipe11_write(Pipe11 *, uint8_t);
extern uint8_t Pipe11_read(Pipe11 *);
extern void Pipe1N_init(Pipe1N *, uint8_t *, uint8_t);
extern void Pipe1N_write(Pipe1N *, uint8_t);
extern uint8_t Pipe1N_read(Pipe1N *);
extern void PipeN1_init(PipeN1 *, uint8_t *, uint8_t);
extern void PipeN1_write(PipeN1 *, uint8_t);
extern uint8_t PipeN1_read(PipeN1 *);
extern void PipeNN_init(PipeNN *, uint8_t *, uint8_t);
extern void PipeNN_write(PipeNN *, uint8_t);
extern uint8_t PipeNN_read(PipeNN *);




//-----------------------------------------------------------------------------
#endif

