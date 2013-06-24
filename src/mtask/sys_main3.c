#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

extern void startMultitasking(void);

void main(void) __attribute__(( noreturn ));
void main(void)
{
	startMultitasking();
	asm volatile ("nop");

	uint16_t i = 0;
	for (; i < 4000; i++)
	{
		_delay_ms(10);
	}
	cli();
	MCUCR |= (1 << SE);
	asm volatile ("sleep");
	for (;;);
}

