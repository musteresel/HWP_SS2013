#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "util/attribute.h"
#include "sensor/incremental.h"




//-----------------------------------------------------------------------------
/** Wheel diameter in mm.
 * */
#define DIAMETER_MM 51




/** Pulses per 360 degree rotation.
 *
 * TODO
 * */
#define PULSE_ROT (24 * 4)




//-----------------------------------------------------------------------------
/** Internal structure to store ticks by the sensors.
 * */
typedef struct __IncrementalTicks
{
	int8_t left;
	int8_t right;
} IncrementalTicks;




//-----------------------------------------------------------------------------
/** History values for both incremental sensors.
 * */
static uint8_t incHistory1;
static uint8_t incHistory2;




/** Variable to store ticks produced by both incremental sensors.
 * */
static volatile IncrementalTicks incrementalTicks;





//-----------------------------------------------------------------------------
/** Constructor to setup pins to receive pin change interrupts.
 * */
ATTRIBUTE( constructor, used) void Incremental_ctor(void)
{
	// Enable pin change interrupt for PCINT0:7 and PCINT8:15
	PCICR |= (1 << PCIE0) | (1 << PCIE1);
	// Enable pc interrupt for num 4,5
	PCMSK0 |= (1 << PCINT4) | (1 << PCINT5);
	PCMSK1 |= (1 << PCINT13) | (1 << PCINT14);
	// Set pins as input
	DDRB &= ~((1 << PB4) | (1 << PB5));
	DDRJ &= ~((1 << PJ4) | (1 << PJ5));
	// Enable pullup resitors
	PORTB |= (1 << PB4) | (1 << PB5);
	PORTJ |= (1 << PJ4) | (1 << PJ5);
	// Set history
	incHistory1 = 0;
	incHistory2 = 0;
}




//-----------------------------------------------------------------------------
/** Interrupt handler, called whenever one of the 4 pins changes state.
 *
 * When the incremental sensor is turned clockwise, it changes its state
 * like this:
 * AB ---> AB |
 * 01      00 | 4
 * 00      10 | 2
 * 10      11 | 11
 * 11      01 | 13
 *
 * When turning counter clockwise, the AB pairs from above change places:
 * AB ---> AB |
 * 00      01 | 1
 * 01      11 | 7
 * 11      10 | 14
 * 10      00 | 8
 *
 * In case A and B are switched, clockwise and counter clockwise are exchanged.
 *
 * Timing diagram:
 * 	---> CW
 * 	<--- CCW
 *
 *      |-OFF-|     |-----
 * A ---|     |-ON--|
 *
 *        |-OFF-|     |----
 * B -----|     |-ON--|
 *
 * */
ISR(PCINT1_vect)
{
	// Read state of pins, TODO: use volatile?
	uint8_t pinstate = PINJ;
	// Isolate AB of sensor
	uint8_t incState2 = pinstate & ((1 << PJ4) | (1 << PJ5));
	// Shift so that A and B are two least significant bits
	incState2 >>= PJ4;
	incState2 &= 0x3;
	// Combine with history
	uint8_t combined2 = (incHistory2 << 2) | incState2;
	// Update tick information of sensor
	switch (combined2)
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
	// Update history
	incHistory2 = incState2;
}
ISR(PCINT0_vect)
{
	// Read state of pins, TODO: use volatile?
	uint8_t pinstate = PINB;
	// Isolate AB of sensor
	uint8_t incState1 = pinstate & ((1 << PB4) | (1 << PB5));
	// Shift so that A and B are two least significant bits
	incState1 >>= PB4;
	incState1 &= 0x3;
	// Combine with history
	uint8_t combined1 = (incHistory1 << 2) | incState1;
	// Update tick information of sensor	
	switch (combined1)
	{
		case 4:
		case 2:
		case 11:
		case 13:
			incrementalTicks.right--;
			break;
		case 7:
		case 14:
		case 8:
		case 1:
			incrementalTicks.right++;
			break;
		default:
			break;
	}
	// Update history
	incHistory1 = incState1;
}




//-----------------------------------------------------------------------------
#define PI 3.14159265359d
WheelDistance Incremental_getDistance(void)
{
	IncrementalTicks data;
	// Protect this area
	cli();
	data.left = incrementalTicks.left;
	data.right = incrementalTicks.right;
	// Reset ticks to zero!
	incrementalTicks.left = incrementalTicks.right = 0;
	sei();
	WheelDistance distance;
	distance.left = (data.left * DIAMETER_MM * PI )/ (double) PULSE_ROT;
	distance.right = (data.right * DIAMETER_MM * PI )/ (double) PULSE_ROT;
	return distance;
}

