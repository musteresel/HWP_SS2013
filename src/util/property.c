#include <stdint.h>
#include <avr/interrupt.h>
#include <util/atomic.h>




/** This is a very stupid implementation ...
 * */
void Property_copy(uint8_t * dest, uint8_t * src, uint8_t count)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		while (count--)
		{
			*dest++ = *src++;
		}
	}
}

