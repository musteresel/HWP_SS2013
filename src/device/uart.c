#include <avr/io.h>
#include <avr/interrupt.h>
#include "util/ringbuffer.h"
#include "kernel/semaphore.h"

#define UART_BUFFER_SIZE 64
#define BAUD_RATE0 38400
#define BAUD_RATE1 38400
#define BAUD_RATE2 38400
#define BAUD_RATE3 38400


//-----------------------------------------------------------------------------
typedef struct __UartRegisterSet_t
{
	uint8_t * UCSRB;
	uint8_t * UDR;
} UartRegisterSet;




typedef struct __Uart_t
{
	Ringbuffer rxBuffer;
	Ringbuffer txBuffer;
	UartRegisterSet reg;
	uint8_t txIntMask;
	Semaphore rxEmptyCount;
	Semaphore rxFillCount;
	Semaphore rxLock;
	Semaphore txEmptyCount;
	Semaphore txFillCount;
	Semaphore txLock;
} Uart;




//-----------------------------------------------------------------------------
#define UART_DATA(n) \
	static uint8_t uart ## n ## Buffer[2][UART_BUFFER_SIZE]; \
	static Uart uart ## n;
UART_DATA(0)
UART_DATA(1)
UART_DATA(2)
UART_DATA(3)




#define UART_INIT(n) \
	uart	= &(uart ## n); \
	uart->reg.UCSRB = (uint8_t *)&UCSR ## n ## B; \
	uart->reg.UDR = (uint8_t *)&UDR ## n; \
	uart->txIntMask = (1 << UDRIE ## n); \
	Ringbuffer_init(&(uart->rxBuffer),uart ## n ## Buffer[0],UART_BUFFER_SIZE); \
	Ringbuffer_init(&(uart->txBuffer),uart ## n ## Buffer[1],UART_BUFFER_SIZE); \
	Semaphore_init(&(uart->rxEmptyCount),UART_BUFFER_SIZE); \
	Semaphore_init(&(uart->rxFillCount),0); \
	Semaphore_init(&(uart->rxLock),1); \
	Semaphore_init(&(uart->txEmptyCount),UART_BUFFER_SIZE); \
	Semaphore_init(&(uart->txFillCount),0); \
	Semaphore_init(&(uart->txLock),1); \
	UBRR ## n ## L = (uint8_t)(F_CPU / (BAUD_RATE ## n * 16L) - 1); \
	UBRR ## n ## H = (F_CPU / (BAUD_RATE ## n * 16L) - 1) >> 8; \
	UCSR ## n ## A = 0x00; \
	UCSR ## n ## B = \
		uart->txIntMask | (1 << RXCIE ## n) | \
		(1 << TXEN ## n) | (1 << RXEN ## n);
void Uart_init(void)
{
	Uart * uart;
	UART_INIT(0);
	UART_INIT(1);
	UART_INIT(2);
	UART_INIT(3);
}




static void Uart_transmit(Uart * uart, uint8_t data)
{
	Semaphore_wait(&(uart->txEmptyCount));
	Semaphore_wait(&(uart->txLock));
	Ringbuffer_put(&(uart->txBuffer),data);
	*(uart->reg.UCSRB) |= uart->txIntMask;
	Semaphore_signal(&(uart->txLock));
	Semaphore_signal(&(uart->txFillCount));
}




static uint8_t Uart_receive(Uart * uart)
{
	uint8_t data;
	Semaphore_wait(&(uart->rxFillCount));
	Semaphore_wait(&(uart->rxLock));
	data = Ringbuffer_get(&(uart->rxBuffer));
	Semaphore_signal(&(uart->rxLock));
	Semaphore_signal(&(uart->rxEmptyCount));
	return data;
}




#define UART_TRANSCEIVER_FCTS(n) \
	void Uart ## n ## _transmit(uint8_t data) \
	{ \
		Uart_transmit(&(uart ## n), data); \
	} \
	uint8_t Uart ## n ## _receive(void) \
	{ \
		return Uart_receive(&(uart ## n)); \
	}
UART_TRANSCEIVER_FCTS(0)
UART_TRANSCEIVER_FCTS(1)
UART_TRANSCEIVER_FCTS(2)
UART_TRANSCEIVER_FCTS(3)




static void Uart_rxHandler(Uart * uart)
{
	// Get byte
	uint8_t data = *(uart->reg.UDR);
	// Use nonblocking try!
	if (Semaphore_try(&(uart->rxEmptyCount)))
	{
		// Put byte into buffer
		Ringbuffer_put(&(uart->rxBuffer), data);
		Semaphore_signal(&(uart->rxFillCount));
	}
	else
	{
		// Buffer is full, so we will lose this byte :/
	}
}




static void Uart_txHandler(Uart * uart)
{
	// Use nonblocking try!
	if (Semaphore_try(&(uart->txFillCount)))
	{
		// Get next byte and write to UDR
		*(uart->reg.UDR) = Ringbuffer_get(&(uart->txBuffer));
		Semaphore_signal(&(uart->txEmptyCount));
	}
	else
	{
		// Disable the transmit interrupt if there is nothing to be
		// transmitted
		*(uart->reg.UCSRB) &= ~(uart->txIntMask);
	}
}




#define UART_ISRS(n) \
	ISR(USART ## n ## _RX_vect) \
	{ \
		Uart_rxHandler(&(uart ## n)); \
	} \
	ISR(USART ## n ## _UDRE_vect) \
	{ \
		Uart_txHandler(&(uart ## n)); \
	}
UART_ISRS(0)
UART_ISRS(1)
UART_ISRS(2)
UART_ISRS(3)

