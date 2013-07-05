#ifndef __KERNEL__SEMAPHORE_H__
#define __KERNEL__SEMAPHORE_H__ 1
#include <stdint.h>
#include "kernel/task.h"
#include "util/attribute.h"




//-----------------------------------------------------------------------------
/** Macro to create a static available semaphore with an initial count.
 * */
#define SEMAPHORE_STATIC(name, count) \
	NOINIT Semaphore name; \
	ATTRIBUTE( constructor, used ) \
	static void Semaphore_ctor_ ## name (void) \
	{ \
		Semaphore_init(&name, count); \
	}




//-----------------------------------------------------------------------------
/** Semaphore structure, including a task wait list and a count
 * */
typedef struct __Semaphore_t
{
	Task * waiting;
	uint8_t count;
} Semaphore;




//-----------------------------------------------------------------------------
/** Try on a semaphore
 * */
extern uint8_t Semaphore_try(Semaphore *);




/** Wait on a semaphore
 * */
extern void Semaphore_wait(Semaphore *);




/** Signal a semaphore
 * */
extern void Semaphore_signal(Semaphore *);




/** Init a semaphore, must be done during startup
 * */
extern void Semaphore_init(Semaphore *, uint8_t );




//-----------------------------------------------------------------------------
#endif

