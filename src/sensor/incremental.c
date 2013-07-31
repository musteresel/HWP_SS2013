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
#define PULSE_ROT_R (24 * 4)
#define PULSE_ROT_L (24 * 4)



//-----------------------------------------------------------------------------
/** Internal structure to store ticks by the sensors.
 * */
typedef struct __IncrementalTicks
{
	int8_t left;
	int8_t right;
} IncrementalTicks;




/** Variable to store ticks produced by both incremental sensors.
 * */
static volatile IncrementalTicks incrementalTicks;



static uint8_t history1;
static uint8_t history2;




//-----------------------------------------------------------------------------
/** Constructor to setup pins to receive pin change interrupts.
 * */
ATTRIBUTE( constructor, used) void Incremental_ctor(void)
{
	DDRB &= ~((1 << PB6) | (1 << PB7));
	DDRJ &= ~((1 << PJ4) | (1 << PJ5));
	PORTB |= (1 << PB6) | (1 << PB7);
	PORTJ |= (1 << PJ4) | (1 << PJ5);
	PRR0 &= ~(1 << PRTIM2);
	ASSR  = 0;
	OCR2A = 125;
	TIMSK2 |= (1 << OCIE2A);
	TCCR2B = 0;
	TCNT2 = 0;
	TCCR2A = (1 << WGM21);
	TCCR2B =  (1 << CS21) | (1 << CS20);
	history1 = 0;
	history2 = 0;
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
	distance.left = (data.left * DIAMETER_MM * PI )/ (double) PULSE_ROT_L;
	distance.right = (data.right * DIAMETER_MM * PI )/ (double) PULSE_ROT_R;
	return distance;
}





/*
 * AB --> AB CW CCW
 * 00     10 2    8
 * 10     11 11  14
 * 11     01 13   7
 * 01     00 4    1
*/

static const int8_t tab[16] =
{
	+0,
	-1,
	+1,
	+0,
	+1,
	+0,
	+0,
	-1,
	-1,
	+0,
	+0,
	+1,
	+0,
	+1,
	-1,
	+0
};

ISR(TIMER2_COMPA_vect)
{
	register uint8_t pinB = PINB;
	register uint8_t pinJ = PINJ;
	register uint8_t state1 = pinB & ((1 << PB6) | (1 << PB7));
	register uint8_t state2 = pinJ & ((1 << PJ4) | (1 << PJ5));
	state1 >>= PB6;
	state1 &= 0x3;
	state2 >>= PJ4;
	state2 &= 0x3;
	incrementalTicks.left += tab[(history1 << 2) | state1];
	incrementalTicks.right += tab[(history2 << 2) | state2];
	history1 = state1;
	history2 = state2;
}



