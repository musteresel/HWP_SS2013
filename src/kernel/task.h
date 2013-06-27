/** Multitasking
 * */
#ifndef __KERNEL_TASK_H__
#define __KERNEL_TASK_H__
//-----------------------------------------------------------------------------
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
extern void _Task_setReady(Task *);
/** Unqueue a task. DISABLE INTERRUPTS!
 *
 * Normally, this won't be needed.
 * */
extern void _Task_setNotReady(Task *);
/** Switch to highest priority task in ready queue. DISABLE INTERRUPTS!
 *
 * Don't touch!
 * */
extern Task * _Task_getNextReady(void);
/** Enforce equal timeslice scheduling. DISABLE INTERRUPTS!
 *
 * Don't touch!
 * */
extern time_t _Task_enforceTimeslice(void);
//-----------------------------------------------------------------------------
#endif
