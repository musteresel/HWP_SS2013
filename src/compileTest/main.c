
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>


extern void Motor_init();
extern void Motor_setVelocity_MotorA(int16_t v);
extern void Motor_setVelocity_MotorB(int16_t v);


int main(void) __attribute__ ((noreturn));
int main(void)
{

	Motor_init();
	Motor_setVelocity_MotorA(150);
	Motor_setVelocity_MotorB(-150);
	unsigned char i = 0;
	for (;i < 200; i++)
	{
		_delay_ms(10);
	}
	Motor_setVelocity_MotorA(0);
	Motor_setVelocity_MotorB(0);


	DDRJ = 0xFF;
	for(;;)
	{
		unsigned char i = 0;
		for (;i < 100; i++)
		{
			_delay_ms(10);
		}
		PORTJ ^= (1 << PJ3);
	}
}



