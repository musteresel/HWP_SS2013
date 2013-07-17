#include <avr/io.h>
#include "util/attribute.h"



//-----------------------------------------------------------------------------
/** Init timer4 to fast pwm mode
 * */
static void timer4_init(void)
{
	TCCR4B = 0x00;
	TCNT4H = 0xFF;
	TCNT4L = 0x01;
	OCR4AH = 0x00;
	OCR4AL = 0x00;
	OCR4BH = 0x00;
	OCR4BL = 0x00;
	OCR4CH = 0x00;
	OCR4CL = 0xFF;
	ICR4H  = 0x00;
	ICR4L  = 0xFF;
	TCCR4A = 0xA1;
	TCCR4C = 0x00;
	TCCR4B = 0x0A;
}




/** Init timer5 to fast pwm mode
 * */
static void timer5_init(void)
{
	TCCR5B = 0x00;
	TCNT5H = 0xFF;
	TCNT5L = 0x01;
	OCR5AH = 0x00;
	OCR5AL = 0x00;
	OCR5BH = 0x00;
	OCR5BL = 0x00;
	OCR5CH = 0x00;
	OCR5CL = 0xFF;
	ICR5H  = 0x00;
	ICR5L  = 0xFF;
	TCCR5A = 0xA1;
	TCCR5C = 0x00;
	TCCR5B = 0x0A;
}




//-----------------------------------------------------------------------------
/** Constructor to setup both timers for the engines.
 * */
ATTRIBUTE( constructor, used) static void Motor_ctor(void)
{
	DDRH |= (1<<3) | (1<<4); //OC4A,B as output
	DDRL |= (1<<3) | (1<<4); //OC5A,B as output

	timer4_init();
	timer5_init();
}




//-----------------------------------------------------------------------------
void Motor_set4(int16_t velocity)
{
	if (velocity >= 0)
	{
		OCR4AL = velocity & 0xFF;
		OCR4BL = 0x00;
	}
	else
	{
		OCR4AL = 0x00;
		OCR4BL = (-velocity) & 0xFF;
	}
}




void Motor_set5(int16_t velocity)
{
	if (velocity >= 0)
	{
		if (velocity > 253)
		{
			velocity = 253;
		}
		OCR5AL = velocity & 0xFF;
		OCR5BL = 0x00;
	}
	else
	{
		if (velocity < -253)
		{
			velocity = -253;
		}
		OCR5AL = 0x00;
		OCR5BL = (-velocity) & 0xFF;
	}
}

