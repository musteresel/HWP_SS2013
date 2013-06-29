#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "kernel/task.h"
#include "IO/ADC/ADC.h"
#include "sensor/ir.h"
#include "communication/communication.h"
#include "IO/uart/uart.h"


volatile uint16_t a;

Task irReaderTask;
uint8_t irReaderStack[200];

void irReader(void) __attribute__((noreturn));
void irReader(void)
{
	dist_t front,left,right;
	a = 0;
	do
	{
		Task_waitCurrent(5);
		_delay_ms(12);
		cli();
		a++;
		sei();
		asm volatile ("nop");/*		front = readIR(IR_FRONT);
		left = readIR(IR_LEFT);
		right = readIR(IR_RIGHT);
		cli();
		communication_log(LEVEL_FINE, "F: %5u L:%5u R:%5u END",
				front, left, right);
		sei();
		PORTJ ^= (1 << PJ3);*/
	} while (a<65535);
	for (;;);
}



#define LEDFCT(x) \
	Task ledTask ## x; \
	uint8_t ledTaskStack ## x[100]; \
	void ledFct ## x (void) \
{ \
	do \
	{ \
		_delay_ms(x); \
		asm volatile ("nop"); \
		Task_waitCurrent(1 << x); \
		asm volatile ("nop"); \
	} while (1); \
}

#define INITLED(x) \
	ledTask ## x .priority = x >> 1; \
	Task_init(&ledTask ## x , ledFct ## x , &(ledTaskStack ## x [99])); \
	_Task_setReady(&ledTask ## x );
	
LEDFCT(0)
LEDFCT(1)
LEDFCT(2)
LEDFCT(3)
LEDFCT(4)
LEDFCT(5)
LEDFCT(6)
LEDFCT(7)
LEDFCT(8)
LEDFCT(9)
LEDFCT(10)
LEDFCT(11)
LEDFCT(12)
LEDFCT(13)
LEDFCT(14)



Task dummy;
uint8_t dummyStack[100];
void dummyFct(void) __attribute__((noreturn));
void dummyFct(void)
{
	do
	{
		_delay_ms(3);
		Task_waitCurrent(2);
		asm volatile ("nop");
		cli();
		if (a > 65534)
		{
			asm volatile ("sleep");
		}
	} while (1);
};



void main(void) __attribute__(( naked ));
void main(void)
{
	//ADC_init();
	//uart_init();
	//communication_init();

	/*DDRK &= ~((1 << PK5) | (1 << PK6) | (1 << PK7));
	PORTK &= ~((1 << PK5) | (1 << PK6) | (1 << PK7));*/
	//communication_log(LEVEL_FINE, "Booting");
	DDRJ = 0 | (1 << PJ3);

	/*
	irReaderTask.priority = 6;
	Task_init(&irReaderTask, irReader, &(irReaderStack[199]));
	_Task_setReady(&irReaderTask);

	dummy.priority = 3;
	Task_init(&dummy, dummyFct, &(dummyStack[99]));
	_Task_setReady(&dummy);
*/

	INITLED(0);
	INITLED(1);
	INITLED(2);
	INITLED(3);
	INITLED(4);
	INITLED(5);
	INITLED(6);
	INITLED(7);
	INITLED(8);
	INITLED(9);
	INITLED(10);
	INITLED(11);
	INITLED(12);
	INITLED(13);
	INITLED(14);
	
	Multitasking_init();
}

