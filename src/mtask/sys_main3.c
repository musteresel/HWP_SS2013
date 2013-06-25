#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "mtask/task.h"
#include "IO/ADC/ADC.h"
#include "sensor/ir.h"
#include "communication/communication.h"
#include "IO/uart/uart.h"


Task irReaderTask;
uint8_t irReaderStack[200];

void irReader(void) __attribute__((noreturn));
void irReader(void)
{
	dist_t front,left,right;
	do
	{
		front = readIR(IR_FRONT);
		left = readIR(IR_LEFT);
		right = readIR(IR_RIGHT);
		uint8_t i = 0;
		for (; i < 50; i++)
		{
			_delay_ms(10);
		}
		cli();
		communication_log(LEVEL_FINE, "F: %5u L:%5u R:%5u END",
				front, left, right);
		sei();
		PORTJ ^= (1 << PJ3);
	} while (1);
}




void main(void) __attribute__(( naked ));
void main(void)
{
	ADC_init();
	uart_init();
	communication_init();

	/*DDRK &= ~((1 << PK5) | (1 << PK6) | (1 << PK7));
	PORTK &= ~((1 << PK5) | (1 << PK6) | (1 << PK7));*/
	communication_log(LEVEL_FINE, "Booting");
	DDRJ = 0 | (1 << PJ3);

	irReaderTask.priority = 6;
	initTask(&irReaderTask, irReader, &(irReaderStack[199]));
	setTaskReady(&irReaderTask);

	startMultitasking();
}

