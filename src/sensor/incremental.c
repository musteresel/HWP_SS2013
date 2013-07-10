#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "util/attribute.h"



ATTRIBUTE( constructor, used) void Incremental_ctor(void)
{
	// Enable pin change interrupt for PCINT0:7
	PCICR |= (1 << PCIE0);
	// Enable pc interrupt for num 4,5,6,7
	PCMSK0 |= (1 << PCINT4) | (1 << PCINT5) | (1 << PCINT6) | (1 << PCINT7);
	// Set pins as input
	DDRB &= ~((1 << PB4) | (1 << PB5) | (1 << PB6) | (1 << PB7));
	// Enable pullup resitors
	PORTB |= (1 << PB4) | (1 << PB5) | (1 << PB6) | (1 << PB7);
	// Set history
	incHistory1 = 0;
	incHistory2 = 0;
}

static uint8_t incHistory1;
static uint8_t incHistory2;

typedef struct __IncrementalTicks
{
	int8_t left;
	int8_t right;
} IncrementalTicks;

volatile IncrementalTicks incrementalTicks;


ISR(PCINT0_vect)
{
	// Read state of pins, TODO: use volatile?
	uint8_t pinstate = PINB;
	uint8_t incState1 = pinstate & ((1 << PB4) | (1 << PB5));
	incState1 >>= PB4;
	uint8_t incState2 = pinstate & ((1 << PB6) | (1 << PB7));
	incState2 >>= PB6;
	uint8_t combined1 = (incHistory1 << 2) | incState1;
	uint8_t combined2 = (incHistory2 << 2) | incState2;
	switch (combined1)
	{
		case 4:
		case 2:
		case 11:
		case 13:
			incrementalTicks.left++;
			break;
		case 7:
		case 14:
		case 8:
		case 1:
			incrementalTicks.left--;
			break;
		default:
			break;
	}
	switch (combined2)
	{
		case 4:
		case 2:
		case 11:
		case 13:
			incrementalTicks.right++;
			break;
		case 7:
		case 14:
		case 8:
		case 1:
			incrementalTicks.right--;
			break;
		default:
			break;
	}
	incHistory1 = incState1;
	incHistory2 = incState2;
}

typedef struct __WheelDistance_t
{
	int8_t left;
	int8_t right;
} WheelDistance;




WheelDistance Incremental_getDistance(void)
{
	IncrementalTicks data;
	cli();
	data.left = incrementalTicks.left;
	data.right = incrementalTicks.right;
	incrementalTicks.left = incrementalTicks.right = 0;
	sei();
	WhellDistance distance;
	distance.left = data.left * (5 * 3.14);
	distnace.right = data.right * (5 * 3.14);
	return distance;
}





