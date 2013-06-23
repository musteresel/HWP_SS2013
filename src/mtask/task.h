/* Multitasking
 *
 * */
#ifndef __TASK_H__
#define __TASK_H__


#define STACK_SIZE 200;


typedef void (*TaskFct)(void);
typedef volatile struct __Task_type
{
	volatile uint8_t sreg;
	volatile uint8_t * sp;
	volatile uint8_t stack[STACK_SIZE];
} Task;

extern volatile uint8_t currentTask;
extern void Task_create(Task *, TaskFct, uint8_t);



extern TaskId exec(TaskFct, uint8_t, uint8_t);
extern void pause(TaskId);
extern void resume(TaskId);
extern void kill(TaskId);


#endif
