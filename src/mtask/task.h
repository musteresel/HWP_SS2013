/* Multitasking
 *
 * */
#ifndef __TASK_H__
#define __TASK_H__


#define PRESCALER 64u
#define COUNT_MILLISECOND ((F_CPU/PRESCALER)/1000u) // with 8Mhz => 125

#define MAX_RR_TIME 10u // MilliSeconds
#define MAX_DELAY_TIME ((65536u/COUNT_MILLISECOND)- 1u) // MilliSeconds

typedef uint16_t time_t;
typedef void (*TaskFct)(void);
typedef struct __Task_struct
{
	uint8_t sreg; // Status Register
	uint8_t * sp; // Stack pointer
	time_t rrTime; // Tasks run time (for round robin)
	uint8_t priority; // Task priority (0 - 7)
	time_t wakeTime;
	struct __Task_struct * next;
} Task;



extern void startMultitasking(void) __attribute__(( naked ));
extern void setTaskReady(Task *);
extern void initTask(Task *, TaskFct, uint8_t *);


#endif
