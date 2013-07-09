#ifndef __DEVICE__UART_H__
#define __DEVICE__UART_H__ 1
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "kernel/pipe.h"
#include "util/attribute.h"




//-----------------------------------------------------------------------------
/** UART control block. No need to access this directly!
 * */
typedef struct __Uart_t
{
	uint8_t * UDR;
	uint8_t * UCSRB;
	uint8_t txIntMask;
	Pipe * txPipe;
	Pipe * rxPipe;
} Uart;




//-----------------------------------------------------------------------------
/** Macro to statically init UART number n.
 * */
#define UART_STATIC(n) \
	PIPE_STATIC(uart ## n ## _txPipe, UART ## n ## _TXBUFFER_SIZE); \
	PIPE_STATIC(uart ## n ## _rxPipe, UART ## n ## _RXBUFFER_SIZE); \
	NOINIT static Uart uart ## n; \
	ISR(USART ## n ## _RX_vect) \
	{ \
		Uart_rxHandler(&(uart ## n)); \
	} \
	ISR(USART ## n ## _UDRE_vect) \
	{ \
		Uart_txHandler(&(uart ## n)); \
	} \
	void Uart ## n ## _readyToTransmit(void) \
	{ \
		Uart_readyToTransmit(&(uart ## n)); \
	} \
	ATTRIBUTE( constructor, used ) \
	static void Uart_ctor_ ## n (void) \
	{ \
		Uart * uart = &(uart ## n); \
		uart->UCSRB =  (uint8_t *)&UCSR ## n ## B; \
		uart->UDR = (uint8_t *)&UDR ## n; \
		uart->txIntMask = (1 << UDRIE ## n); \
		uart->txPipe = &(uart ## n ## _txPipe); \
		uart->rxPipe = &(uart ## n ## _rxPipe); \
		UBRR ## n ## L = (uint8_t)(F_CPU / (UART ## n ## _BAUDRATE * 16L) - 1); \
		UBRR ## n ## H = (F_CPU / (UART ## n ## _BAUDRATE * 16L) - 1) >> 8; \
		UCSR ## n ## A = (1 << 5); \
		UCSR ## n ## B = (1 << RXCIE ## n) | \
			(1 << TXEN ## n) | (1 << RXEN ## n); \
		UCSR ## n ## C = 0x86; \
	}




//-----------------------------------------------------------------------------
/** Enables UART ready to transmit interrupt.
 * */
extern void Uart_readyToTransmit(Uart *);




/** Receive handler to be called from receive interrupt.
 *
 * No need to touch this.
 * */
extern void Uart_rxHandler(Uart *);




/** Transmit handler to be called from ready to transmit interrupt.
 *
 * No need to touch this.
 * */
extern void Uart_txHandler(Uart *);




//-----------------------------------------------------------------------------
#endif

