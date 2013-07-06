/** Multitasking
 * */
#ifndef __KERNEL_TASK_H__
#define __KERNEL_TASK_H__ 1
#include <stdint.h>
#include "util/attribute.h"




//-----------------------------------------------------------------------------
/** Macro to create a static available task.
 *
 * name = name of the task
 * prio = priority
 * fct = function name to execute (forward declarated as static!)
 * stacksize = size of stack
 * ready = 0 -> don't set as ready, else set as ready
 * */
#define TASK_STATIC(name, prio, fct, stacksize, ready) \
	NOINIT Task name; \
	NOINIT static uint8_t name ## Stack[stacksize]; \
	static void fct (void); \
	ATTRIBUTE( constructor, used ) \
	void Task_ctor_ ## name (void) \
	{ \
		name.priority = prio; \
		Task_init(&name, fct, &(name ## Stack[stacksize - 1])); \
		if (ready) Task_setReady(&name); \
	}




/** Prescaler in use is 64.
 *
 * Don't change this! The prescaler is hardcoded in task.c
*/
#define TASKTIMER_PRESCALER 64u




//-----------------------------------------------------------------------------
/** Unit of time and time deltas
 * */
typedef uint16_t time_t;




/** Type of function to be used as a task
 * */
typedef void (*TaskFct)(void);




/** Task control block
 *
 * No direct access to any of the members!
 * */
typedef struct __Task_struct
{
	uint8_t * sp;
	struct __Task_struct * next;
	uint8_t priority;
	time_t rrTime;
	time_t wakeTime;
} Task;




//-----------------------------------------------------------------------------
/** Let the current (calling) task wait for time_t milliseconds.
 *
 * This causes the task to be queued in a wait list. There is no guarantee,
 * that the task will be executed after the wait time has passed: There could
 * be another task with higher priority!
 * */
extern void Task_waitCurrent(time_t);




/** Initialize multitasking. Does NOT RETURN!
 * */
extern void Multitasking_init(void);




/** Init a task control block.
 *
 * The third parameter must point to the top of a suitable large stack area.
 * Calling this functio only inits the task control block, but does not queue
 * the task! See _Task_setReady!
 * */
extern void Task_init(Task *, TaskFct, uint8_t *);




/** Queue a task. DISABLE INTERRUPTS!
 * */
extern void Task_setReady(Task *);




/** Unqueue a task. DISABLE INTERRUPTS!
 *
 * Normally, this won't be needed.
 * */
extern void Task_setNotReady(Task *);




/** Switch to highest priority task in ready queue. DISABLE INTERRUPTS!
 *
 * Don't touch!
 * */
extern Task * Task_getNextReady(void);




/** Enforce equal timeslice scheduling. DISABLE INTERRUPTS!
 *
 * Don't touch!
 * */
extern time_t Task_enforceTimeslice(uint8_t priority);




/** Yield execution, will cause the same task to be run if it is still in
 * the ready list.
 * */
extern void Task_yield(void);




/** Return a pointer to the current running task. No need for cli/sei here.
 * */
extern Task * Task_getCurrent(void);




//-----------------------------------------------------------------------------
#endif

