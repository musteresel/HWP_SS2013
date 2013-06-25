#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>



#define PRESCALER 64u
#define COUNT_MILLISECOND ((F_CPU/PRESCALER)/1000u) // with 8Mhz => 125

#define MAX_RR_TIME 10u // MilliSeconds
#define MAX_DELAY_TIME ((65536u/COUNT_MILLISECOND)- 1u) // MilliSeconds
#define STACK_SIZE 128




#define RPUSH(r) asm volatile ("push r"#r);
#define RPOP(r) asm volatile ("pop r"#r);

#define PUSH_REGS() \
	RPUSH(31);RPUSH(30);RPUSH(29);RPUSH(28);RPUSH(27);RPUSH(26);RPUSH(25);\
	RPUSH(24);RPUSH(23);RPUSH(22);RPUSH(21);RPUSH(20);RPUSH(19);RPUSH(18);\
	RPUSH(17);RPUSH(16);RPUSH(15);RPUSH(14);RPUSH(13);RPUSH(12);RPUSH(11);\
	RPUSH(10);RPUSH(9);RPUSH(8);RPUSH(7);RPUSH(6);RPUSH(5);RPUSH(4);\
	RPUSH(3);RPUSH(2);RPUSH(1);RPUSH(0);
#define POP_REGS() \
	RPOP(0);RPOP(1);RPOP(2);RPOP(3);RPOP(4);RPOP(5);RPOP(6);RPOP(7);RPOP(8);\
	RPOP(9);RPOP(10);RPOP(11);RPOP(12);RPOP(13);RPOP(14);RPOP(15);RPOP(16);\
	RPOP(17);RPOP(18);RPOP(19);RPOP(20);RPOP(21);RPOP(22);RPOP(23);RPOP(24);\
	RPOP(25);RPOP(26);RPOP(27);RPOP(28);RPOP(29);RPOP(30);RPOP(31);


#define TIMER_ISR TIMER1_COMPA_vect


typedef uint16_t time_t;
typedef void (*TaskFct)(void);
typedef struct __Task_struct
{
	uint8_t sreg; // Status Register
	uint8_t * sp; // Stack pointer
	uint8_t stack[STACK_SIZE]; // Tasks stack
	time_t rrTime; // Tasks run time (for round robin)
	uint8_t priority; // Task priority (0 - 7)
	time_t wakeTime;
	struct __Task_struct * next;
} Task;


Task * readyTasks[8];
Task * currentTask;
Task * nextTaskToWake;
time_t nextTime;
time_t taskStartTime;



ISR(TIMER_ISR, ISR_NAKED)
{
	PUSH_REGS();
	currentTask->sreg = SREG;
	currentTask->sp = (uint8_t *)(SP);
	// Variables
	time_t currentTime;
	time_t timerDelay;

	currentTime = nextTime;
	currentTask->rrTime += currentTime - taskStartTime;
	timerDelay = MAX_DELAY_TIME;
	
	// Wake tasks whose time has come
	while (nextTaskToWake && currentTime == nextTaskToWake->wakeTime)
	{
		Task * toWake = nextTaskToWake;
		nextTaskToWake = toWake->next;
		if (toWake->priority < currentTask->priority)
		{
			currentTask = toWake;
		}
		if (readyTasks[toWake->priority])
		{
			Task * currentReady = readyTasks[toWake->priority]; // TODO unfair?
			toWake->next = currentReady->next;
			currentReady->next = toWake;
			readyTasks[toWake->priority] = toWake; // unfair?
		}
		else
		{
			readyTasks[toWake->priority] = toWake;
			toWake->next = toWake;
		}
	}

	// Apply round robin scheduling on the next priorities queue
	if (currentTask->next == currentTask)
	{
		// No rr needed if only one task in the queue
		currentTask->rrTime = 0;
		timerDelay = MAX_DELAY_TIME;
	}
	else
	{
		// If tasks timeslice is over, advance to the next in the queue
		while (currentTask->rrTime >= MAX_RR_TIME)
		{
			currentTask->rrTime = 0;
			readyTasks[currentTask->priority] = currentTask->next;
			currentTask = currentTask->next;
		}
		timerDelay = MAX_RR_TIME - currentTask->rrTime; // assert > 0
		if (timerDelay == 0)
		{
			asm volatile ("nop ;Mist");
			for (;;);
		}
	}

	if (nextTaskToWake)
	{
		time_t wakeDelay = nextTaskToWake->wakeTime - currentTime;
		if (wakeDelay < timerDelay)
		{
			timerDelay = wakeDelay;
		}
	}

	OCR1A = timerDelay * COUNT_MILLISECOND;
	nextTime += timerDelay;
	taskStartTime = currentTime;

	SREG = currentTask->sreg;
	SP = (uint16_t)(currentTask->sp);
	POP_REGS();
//	sei();
	asm volatile ("reti");
}

Task idleTask;
Task taskA;
uint8_t stackA[100];


void fctA(void) __attribute__(( noreturn ));
void fctA(void)
{
	asm volatile ("nop");
	RPUSH(1);
	RPUSH(2);
	for (;;);
}

void initTask(Task * t, TaskFct f, uint8_t * s)
{
	s--;
	*s-- = ((uint16_t)f) >> 8;
	*s-- = ((uint16_t)f) & 0xFF;
	uint8_t iterator = 0;
	for (; iterator < 32; iterator++)
	{
		*s-- = 0;
	}
	s--;
	t->sp = s;
	t->sreg = 0;
	t->rrTime = 0;
}




void startMultitasking(void)
{
	readyTasks[0] = readyTasks[1] = readyTasks[2] = readyTasks[3] =
		readyTasks[4] = readyTasks[5] = readyTasks[6] = 0;
	readyTasks[7] = &idleTask;
	nextTaskToWake = &taskA;
	idleTask.priority = 7;
	idleTask.next = &idleTask;
	currentTask = &idleTask;

	taskA.priority = 7;
	initTask(&taskA, fctA, &(stackA[99]));
	taskA.wakeTime = 13;
	taskA.next = 0;


	TCCR1B = 0; // stop timer
	TCNT1 = 0; // reset
	// CTC with prescaler 64
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
	// we call the isr directly
	OCR1A = 65535;
	taskStartTime = 65530;
	nextTime = 65530;
	TIMSK1 |= (1 << OCIE1A);
	TIMER_ISR();
}





