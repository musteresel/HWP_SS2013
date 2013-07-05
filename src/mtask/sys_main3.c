#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "kernel/task.h"
#include "kernel/message.h"
#include "kernel/memory.h"
#include "IO/ADC/ADC.h"
#include "sensor/ir.h"
#include "communication/communication.h"
#include "device/uart.h"
#include "util/attribute.h"


volatile uint16_t a;

#ifdef DEBUG_SIMAVR
#include "avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega1280");
const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
	{ AVR_MCU_VCD_SYMBOL("UDR0"), .what = (void*)&UDR0, },
	{ AVR_MCU_VCD_SYMBOL("UDRE0"), .mask = (1 << UDRE0), .what = (void*)&UCSR0A, },
	{ AVR_MCU_VCD_SYMBOL("UDRIE0"), .mask = (1 << UDRIE0), .what = (void*)&UCSR0B, },
};
#endif


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
		Task_waitCurrent((1 << x)); \
		asm volatile ("nop"); \
	} while (1); \
}

#define INITLED(x) \
	ledTask ## x .priority = x >> 1; \
	Task_init(&ledTask ## x , ledFct ## x , &(ledTaskStack ## x [99])); \
	Task_setReady(&ledTask ## x );
	
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

#include "kernel/pipe.h"

PIPE11_STATIC(myPipe,128);


Task dummy;
uint8_t dummyStack[100];


void printBytes(uint8_t * start, uint8_t size)
{
	while (size)
	{
		Uart0_transmit(*start);
		start++;
		size--;
	}
}


void dummyFct(void) __attribute__((noreturn));
void dummyFct(void)
{
	uint8_t inc = 0;
	do
	{
		Task_waitCurrent(500);
		asm volatile("nop");
		Uart0_transmit('a' + inc++);
		if (inc > 10) 
		{
			inc = 0;
			Uart0_transmit('\n');
		}
	} while (1);
};


ATTRIBUTE( noinline ) void BREAKPOINT(void)
{
	asm volatile("nop; This is a breakpoint");
}

MessageBox testBox;
Task producerA;
Task consumer1;
uint8_t producerAStack[100];
uint8_t consumer1Stack[100];
ATTRIBUTE( noreturn ) void producerAFct(void)
{
	uint8_t inc = 0;
	do
	{
		uint8_t data[3];
		data[0] = 'A';
		data[1] = '1' + inc;
		data[2] = '\n';
		if (inc > 8)
		{
			inc = 0;
		}
		else
		{
			inc++;
		}
		uint8_t str[] = "procudeA\n";
		printBytes(str,sizeof(str));
		Message_send(&testBox,&(data[0]),3);
	} while(1);
}
ATTRIBUTE( noreturn ) void consumer1Fct(void)
{
	do
	{
		Message * msg = Message_receive(&testBox);
		BREAKPOINT();
		uint8_t str[] = "Consumer1 received: ";
		printBytes(str,sizeof(str)-1);
		printBytes(msg->buffer,msg->size);
		Message_free(msg);
	} while(1);
}





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

	*/
	dummy.priority = 0;
	Task_init(&dummy, dummyFct, &(dummyStack[99]));
	Task_setReady(&dummy);

	Message_initBox(&testBox,10);
	producerA.priority = 5;
	consumer1.priority = 3;
	Task_init(&producerA, producerAFct, &(producerAStack[99]));
	Task_init(&consumer1, consumer1Fct, &(consumer1Stack[99]));
	Task_setReady(&producerA);
	Task_setReady(&consumer1);


	Uart_init();
	Memory_init();
/*
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
	INITLED(14);*/
	
	Multitasking_init();
}

