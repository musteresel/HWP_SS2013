#include <stdint.h>
#include "kernel/task.h"
#include "kernel/pipe.h"
#include "device/motor.h"
#include "speed.h"

PIPE_STATIC(translationPipe,10);


static void getTranslation(Translation * t)
{
	uint8_t * write = (uint8_t*)t;
	Pipe_startRead(&translationPipe);
	uint8_t iterator = 0;
	for (; iterator < sizeof(Translation); iterator++)
	{
		Pipe_read(&translationPipe,write);
		write++;
	}
	Pipe_endRead(&translationPipe);
}

void Translation_set(Translation * t)
{
	uint8_t * read = (uint8_t*)t;
	Pipe_startWrite(&translationPipe);
	uint8_t iterator = 0;
	for (; iterator < sizeof(Translation); iterator++)
	{
		Pipe_write(&translationPipe,*read);
		read++;
	}
	Pipe_endWrite(&translationPipe);
}

TASK_STATIC(translator,0,translatorFct,100,1);
static void translatorFct(void)
{
	Translation wanted;
	int16_t left;
	int16_t right;
	do
	{
		getTranslation(&wanted);
		/*if (wanted.steering >= 0)
		{
			left = wanted.speed * wanted.speed + wanted.steering * wanted.steering;
			right = wanted.speed - wanted.steering;
		}
		else
		{
			left = wanted.speed - wanted.steering;
			right = wanted.speed * wanted.speed + wanted.steering * wanted.steering;
		}*/
		left = wanted.speed + wanted.steering;
		right = wanted.speed - wanted.steering;
		Motor_set4(left);
		Motor_set5(right);
	} while (1);
}



