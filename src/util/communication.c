#include <stdint.h>
#include "device/uart.h"
#include "kernel/pipe.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>



//-----------------------------------------------------------------------------
#define ESC 17
#define DELIM '+'




//-----------------------------------------------------------------------------
static uint8_t outPacketCount = 0;
static uint8_t inPacketCount = 0;




//-----------------------------------------------------------------------------
#define UART0_TXBUFFER_SIZE 64
#define UART0_RXBUFFER_SIZE 64
#define UART0_BAUDRATE 38400
UART_STATIC(0);




//-----------------------------------------------------------------------------
/** Escape a byte and transmit.
 * */
static inline void transmitEscapedByte(uint8_t byte)
{
	if ((byte == ESC) | (byte == DELIM))
	{
		Pipe_write(&uart0_txPipe, ESC);
		Uart0_readyToTransmit();
	}
	Pipe_write(&uart0_txPipe, byte);
	Uart0_readyToTransmit();
}




//-----------------------------------------------------------------------------
/** write packet via uart.
 * */
void Communication_writePacket(uint8_t channel, uint8_t * packet, uint8_t size)
{
	uint8_t iterator;
	uint8_t checkSum;
	// Lock the communication channel
	Pipe_startWrite(&uart0_txPipe);
	// Init the checksum using the outPacketCount and the channel
	checkSum = (outPacketCount << 5) | (channel & 0x1F);
	// Transmit channel and packet sequence information
	transmitEscapedByte(checkSum);
	// Transmit packet contents, updating checksum
	for (iterator = 0; iterator < size; iterator++)
	{
		uint8_t data = packet[iterator];
		transmitEscapedByte(data);
		checkSum ^= data;
	}
	// Transmit checksum
	transmitEscapedByte(checkSum);
	// Transmit delimiter
	Pipe_write(&uart0_txPipe, DELIM);
	Uart0_readyToTransmit();
	// Update packet sequence number
	outPacketCount = (outPacketCount + 1) & 0x07;
//	Uart0_readyToTransmit();
	Pipe_endWrite(&uart0_txPipe);
}




/** read a packet from UART
 * */
uint8_t Communication_readPacket(uint8_t * packet, uint8_t maxsize)
{
	uint8_t checkSum = 0;
	uint8_t receiveCount = 0;
	Pipe_startRead(&uart0_rxPipe);
	do
	{
		uint8_t data;
		Pipe_read(&uart0_rxPipe,&data);
		if (data == DELIM)
		{
			if (checkSum == 0 && receiveCount >= 2)
			{
				Pipe_endRead(&uart0_rxPipe);
				packet[0] &= 0x1F;
				return 1;
			}
			else
			{
				return 0;
			}
		}
		if (data == ESC)
		{
			Pipe_read(&uart0_rxPipe,&data);
		}
		packet[receiveCount++] = data;
		checkSum ^= data;
		if (receiveCount == maxsize)
		{
			// Buffer overflow
			// TODO: free until DELIM
			Pipe_endRead(&uart0_rxPipe);
			return 0;
		}
	} while (1);
}



void Communication_log(uint8_t level, const char* format, ...) {
	char buff[81];
	char * message;
	
	va_list argp;
	va_start(argp, format);
	
	message = (buff+1);
	buff[0]=level;
	
	vsnprintf(message,80, format, argp);
	va_end(argp);

	Communication_writePacket(0, (uint8_t *)(&buff[0]), strlen(message)+1);
}


