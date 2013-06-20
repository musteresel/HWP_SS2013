#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


/*
#define save_context() \
	asm volatile ( \
			"push r0 \n\t" \
			"in r0,__SREG__ \n\t" \
			"cli \n\t" \
			"push r0 \n\t" \
			"push r1 \n\t" \
			"clr r1 \n\t" \
			"push r2 \n\t" \
			"push r3 \n\t" \
			"push r4 \n\t" \
			"push r5 \n\t" \
			"push r6 \n\t" \
			"push r7 \n\t" \
			"push r8 \n\t" \
			"push r9 \n\t" \
			"push r10 \n\t" \
			"push r11 \n\t" \
			"push r12 \n\t" \
			"push r13 \n\t" \
			"push r14 \n\t" \
			"push r15 \n\t" \
			"push r16 \n\t" \
			"push r17 \n\t" \
			"push r18 \n\t" \
			"push r19 \n\t" \
			"push r20 \n\t" \
			"push r21 \n\t" \
			"push r22 \n\t" \
			"push r23 \n\t" \
			"push r24 \n\t" \
			"push r25 \n\t" \
			"push r26 \n\t" \
			"push r27 \n\t" \
			"push r28 \n\t" \
			"push r29 \n\t" \
			"push r30 \n\t" \
			"push r31 \n\t" \
			"lds r26, currentStackAddress \n\t" \
			"lds r27, currentStackAddress + 1 \n\t" \
			"in r0, __SP_L__ \n\t" \
			"st x+,r0 \n\t" \
			"in r0, __SP_H__ \n\t" \
			"st x+,r0 \n\t" \
		);
#define load_context() \
	asm volatile ( \
			"lds r26, currentStackAddress \n\t" \
			"lds r27, currentStackAddress + 1 \n\t" \
			"ld r28,x+ \n\t" \
			"out __SP_L__, r28 \n\t" \
			"ld r29,x+ \n\t" \
			"out __SP_H__, r29 \n\t" \
			"pop r31 \n\t" \
			"pop r30 \n\t" \
			"pop r29 \n\t" \
			"pop r28 \n\t" \
			"pop r27 \n\t" \
			"pop r26 \n\t" \
			"pop r25 \n\t" \
			"pop r24 \n\t" \
			"pop r23 \n\t" \
			"pop r22 \n\t" \
			"pop r21 \n\t" \
			"pop r20 \n\t" \
			"pop r19 \n\t" \
			"pop r18 \n\t" \
			"pop r17 \n\t" \
			"pop r16 \n\t" \
			"pop r15 \n\t" \
			"pop r14 \n\t" \
			"pop r13 \n\t" \
			"pop r12 \n\t" \
			"pop r11 \n\t" \
			"pop r10 \n\t" \
			"pop r9 \n\t" \
			"pop r8 \n\t" \
			"pop r7 \n\t" \
			"pop r6 \n\t" \
			"pop r5 \n\t" \
			"pop r4 \n\t" \
			"pop r3 \n\t" \
			"pop r2 \n\t" \
			"pop r1 \n\t" \
			"pop r0 \n\t" \
			"out __SREG__, r0 \n\t" \
			"pop r0 \n\t" \
		);

*/


//void taskA(void) __attribute__(( noreturn ));
void taskA(void)
{
	PORTJ |= (1 << PJ3);
	for(;;){
		asm volatile ("nop");
		asm volatile ("nop");
	}
	for(;;)
	{
		unsigned char i = 0;
		for (;i < 200; i++)
		{
			_delay_ms(10);
		}
		PORTJ ^= (1 << PJ3);
	}
}


void taskB(void) __attribute__(( noreturn ));
void taskB(void)
{
	for(;;)
	{
		unsigned char i = 0;
		for (;i < 20; i++)
		{
			_delay_ms(10);
		}
		PORTJ ^= (1 << PJ3);
	}
}


volatile uintptr_t * volatile currentStackAddress = 10;
volatile uint8_t currentTask;
volatile uintptr_t taskAStackAddress;
volatile uintptr_t taskBStackAddress;



uint8_t taskAStack[200];
uint8_t taskBStack[200];


ISR(PCINT1_vect, ISR_NAKED)
{
	save_context();
	PORTJ ^= (1 << PJ3);
	load_context();
	reti();
	/*if (currentTask == 0)
	{
		*currentStackAddress = taskBStackAddress;
		currentTask = 1;
	}
	else
	{
		*currentStackAddress = taskAStackAddress;
		currentTask = 0;
	}
	load_context();*/
}	


void main(void) __attribute__(( noreturn ));
void main(void)
{
	// memory ?
	taskAStackAddress = &(taskAStack[199]);
	taskBStackAddress = &(taskBStack[100]);
	*currentStackAddress = taskAStackAddress;
	currentTask = 0;

	DDRJ = 0 | (1 << PJ3);
	PORTJ |= (1 << PJ4);
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT13);
	sei();
	// TODO
	taskBStack[131] = SREG;
	taskBStack[132] = SREG;
	taskBStack[133] = SREG;

	save_context();
	load_context();
	taskA();

	for (;;);
}



