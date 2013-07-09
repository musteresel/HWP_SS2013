#include <avr/io.h>
#include "kernel/pipe.h"
#include "device/uart.h"




//-----------------------------------------------------------------------------
void Uart_readyToTransmit(Uart * uart)
{
	*(uart->UCSRB) |= uart->txIntMask;
}




//-----------------------------------------------------------------------------
void Uart_rxHandler(Uart * uart)
{
	uint8_t data = *(uart->UDR);
	if (!Pipe_tryWrite(uart->rxPipe, data))
	{
		// Buffer is full, so we will lose this byte :/
	}
}




void Uart_txHandler(Uart * uart)
{
	if (!Pipe_tryRead(uart->txPipe,uart->UDR))
	{
		// Disable the transmit interrupt if there is nothing to be
		// transmitted
		*(uart->UCSRB) &= ~(uart->txIntMask);
	}
}

