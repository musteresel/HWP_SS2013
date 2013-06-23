#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>



#include <IO/uart/uart.h>
#include <communication/communication.h>
#include <communication/packettypes.h>
#include <IO/ADC/ADC.h>


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





#define TIMER_VECTOR TIMER0_COMPA_vect

typedef void (*task_fct)(void);

typedef volatile struct _task_t
{
	volatile uint8_t sreg;
	volatile uint8_t * sp;
	volatile uint8_t stack[200];
} task_t;

static volatile task_t * cTask;
static volatile task_t * tasks[10];
static volatile uint8_t currentTask;

static volatile task_t mainTask;
static volatile task_t taskA;




void TIMER_VECTOR(void) __attribute__ (( signal, naked ));
ISR(TIMER_VECTOR)
{
	PUSH_REGS();
	cTask->sreg = SREG;
	cTask->sp = (uint8_t *)(SP + 32); // o.O ??
	/*cTask = 0;
	while (!cTask)
	{
		if (currentTask < 9)
		{
			currentTask++;
		}
		else
		{
			currentTask = 0;
		}
		cTask = tasks[currentTask];
	}*/
	if (cTask == &taskA)
	{
		cTask = &mainTask;
	}
	else
	{
		cTask = &taskA;
	}
	SREG = cTask->sreg;
	SP = (uint16_t)(cTask->sp - 32);
	POP_REGS();
	asm volatile ("reti");
}



void createTask(task_t * t, task_fct f, uint8_t pos)
{
	uint8_t * s = (uint8_t *)(t->stack + sizeof(t->stack) - 3);
	t->sp = s;
	uint8_t sIterator = 0;
	for (; sIterator < 32; sIterator++)
	{
		*s-- = 0;
	}
	t->sreg = 0; // mhm ...
	t->stack[sizeof(t->stack)-2] = ((uint16_t)f) >> 8;
	t->stack[sizeof(t->stack)-1] = ((uint16_t)f) & 0xFF;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		tasks[pos] = t;
	}
}	



void fctA(void) __attribute__(( noreturn ));
void fctA(void)
{
	for (;;)
	{
		uint8_t i = 0;
		for (; i < 200; i++)
		{
			_delay_ms(10);
		}
		//ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
		//{
			//cli();
			PORTJ ^= (1 << PJ3);
			//communication_log(LEVEL_FINE,"Ich bin Task A, Hi there");

			//Motor_setVelocity_MotorA(-250);
			//sei();
		//}
	}
}


extern void Motor_init();
extern void Motor_setVelocity_MotorA(int16_t v);
extern void Motor_setVelocity_MotorB(int16_t v);



void commDebug(unsigned char channel, unsigned char* packet, unsigned short size) {
		
		communication_log(LEVEL_FINE, "received %i bytes", size);
}


void main(void) __attribute__(( noreturn ));
void main(void)
{

  // Timer 0 konfigurieren
	//
	TCCR0A = (1<<WGM01); // CTC Modus
	TCCR0B |= (1<<CS00) | (1<<CS02); // Prescaler 1024
	// ((1000000/8)/1000) = 125
	OCR0A = 125-1;
	// Compare Interrupt erlauben
	TIMSK0 |= (1<<OCIE0A);

	Motor_init();
	uart_init();
	ADC_init();
	communication_init();
	communication_ChannelReceivers[0]=&commDebug;
	communication_log(LEVEL_INFO, "Booting");
	// Set all tasks to 0
	{
		uint8_t i = 0;
		for (; i < 10; i++)
		{
			tasks[i] = 0;
		}
	}
	// Set maintask as the current task
	currentTask = 0;
	cTask = &mainTask;
	tasks[currentTask] = cTask;
	createTask(&taskA, fctA, 1);
	tasks[1] = 0;

	DDRJ = 0 | (1 << PJ3);
	DDRK |= (1 << PK0);
	PORTJ |= (1 << PJ4);
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT13);
	
	PORTJ &= ~(1 << PJ3);

	sei();
	uint8_t div = 1;
	for (;;)
	{
		//asm volatile ("call __vector_10");
		uint8_t i = 0;
		for (;i < 250;i++)
		{
			_delay_ms(10);
		}
		//cli();
		//Motor_setVelocity_MotorA(-250);
		//Motor_setVelocity_MotorB(250);
		PORTK ^= (1 << PK0);
		communication_log(LEVEL_INFO, "FRONT: %d LEFT: %d RIGHT: %d END",
				ADC_getCurrentValue(0),ADC_getCurrentValue(2),ADC_getCurrentValue(1));
		//sei();
		div++;
		if (div >= 100)
		{
			div = 1;
		}
	}
	//asm volatile ("cli");
	MCUCR |= (1 << SE);
	asm volatile ("sleep");
	for (;;);
}
