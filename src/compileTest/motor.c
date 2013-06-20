
#include <util/delay.h>
#include <avr/io.h>
//#include <avr/interrupt.h>

//TIMER4 initialize - prescale:8
// WGM: 5) PWM 8bit fast, TOP=0x00FF
// desired value: 1KHz
// actual value:  3,906KHz (74,4%)
void timer4_init(void)
{
	//PRR1&=~(1<<PRTIM4);

	TCCR4B = 0x00; //stop
	TCNT4H = 0xFF; //setup
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
	TCCR4B = 0x0A; //start Timer
}

//TIMER5 initialize - prescale:8
// WGM: 5) PWM 8bit fast, TOP=0x00FF
// desired value: 1KHz
// actual value:  3,906KHz (74,4%)
void timer5_init(void)
{
	//PRR1&=~(1<<PRTIM5);

	TCCR5B = 0x00; //stop
	TCNT5H = 0xFF; //setup
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
	TCCR5B = 0x0A; //start Timer
}

void Motor_init()
{
	DDRH |= (1<<3) | (1<<4); //OC4A,B as output
	DDRL |= (1<<3) | (1<<4); //OC5A,B as output

	timer4_init();
	timer5_init();
}

void Motor_setVelocity_MotorA(int16_t velocity)
{
	if (velocity > 0)
	{
		OCR5AL = (velocity & 0xFF);
		OCR5BL = 0x00;
	}
	else
	{
		OCR5AL = 0x00;
		OCR5BL = ((-velocity) & 0xFF);
	}
}

void Motor_setVelocity_MotorB(int16_t velocity)
{
	if (velocity > 0)
	{
		OCR4AL = (velocity & 0xFF);
		OCR4BL = 0x00;
	}
	else
	{
		OCR4AL = 0x00;
		OCR4BL = ((-velocity) & 0xFF);
	}
}

