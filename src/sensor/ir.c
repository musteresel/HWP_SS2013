#include "sensor/ir.h"
#include "IO/ADC/ADC.h"

const dist_t lut[] = 
{
	104, 80, 70, 64, 59, 55, 52, 50, 47, 45, 44, 42, 40, 39, 38, 36, 35, 34, 33,
	32, 31, 31, 30, 29, 28, 27, 27, 26, 25, 25, 24, 24, 23, 22, 22, 21, 21, 20,
	20, 20, 19, 19, 18, 18, 17, 17, 17, 16, 16, 15, 15, 15, 14, 14, 14, 13, 13,
	13, 12, 12, 12, 12, 11, 11, 11, 10, 10  , 10, 10, 9, 9, 9, 9, 8, 8
};

dist_t readIR(uint8_t pos)
{
	uint16_t adc = ADC_getCurrentValue(pos);
	uint8_t adcR = adc & 0x07;
	adc >>= 3;
	if (adc > 86u)
	{
		return FROM_CM(4u);
	}
	if (adc < 13u)
	{
		return lut[0];
	}
	adc -= 13u;
	if (adc < 16u && adcR)
	{
		return (adcR * lut[adc+1u] + (8-adcR) * lut[adc]) >> 3;
	}
	else
	{
		return lut[adc];
	}
}




