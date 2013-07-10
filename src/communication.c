#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "kernel/task.h"
#include "kernel/pipe.h"
#include "util/ringbuffer.h"
#include "util/attribute.h"




//-----------------------------------------------------------------------------
/** Size of the receive buffer. This could be too small, depending on the
 * frequency of incomming packets!
 * */
#define RECEIVE_BUFFER_SIZE 250




/** Size of the transmit buffer. This must be at least twice as large as the
 * max packet size + 3.
 * */
#define TRANSMIT_BUFFER_SIZE 64




/** The acutal buffers and their ringbuffer interfaces.
 * */
NOINIT static uint8_t _receiveBuffer[RECEIVE_BUFFER_SIZE];
NOINIT static uint8_t _transmitBuffer[TRANSMIT_BUFFER_SIZE];
NOINIT static Ringbuffer receiveBuffer;
NOINIT static Ringbuffer transmitBuffer;




/** Pipe to write to the transmitter task.
 * */
PIPE_STATIC(communicationTxPipe,32);




/** Transmitter task.
 *
 * This task reads packets from a pipe and translate one at a time to sendable
 * packets. Then, it enables the uart send interrupt, to get the packet out.
 * */
TASK_STATIC(transmitter,2,transmitterTask,100,1);




/** Statistic counters
 * */
NOINIT static uint8_t outPacketCount;




/** Baudrate used by the communication link
 * */
#define BAUDRATE 38400




/** Special characters
 * */
#define DELIM '+'
#define ESC 17




//-----------------------------------------------------------------------------
/** Constructor to initialize communication subsystem.
 * */
ATTRIBUTE( constructor, used) static void Communication_ctor(void)
{
	outPacketCount = 0;
	// Init the ringbuffers
	Ringbuffer_init(&receiveBuffer, _receiveBuffer, RECEIVE_BUFFER_SIZE);
	Ringbuffer_init(&transmitBuffer, _transmitBuffer, TRANSMIT_BUFFER_SIZE);
	// Set uart
	UBRR0L = (uint8_t)(F_CPU / (BAUDRATE * 16L) - 1);
	UBRR0H = (F_CPU / (BAUDRATE * 16L) - 1) >> 8;
	UCSR0A = (1 << 5);
	UCSR0B = (1 << RXCIE0) | (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = 0x86;
}




//-----------------------------------------------------------------------------
/** Signal that the uart can begin to transmit the packet which is in the 
 * transmit buffer.
 * */
static void readyToTransmit(void)
{
	// Enable UDR ready interrupt
	UCSR0B |= (1 << UDRIE0);
}




/** Put a byte into the transmit buffer,  escaping it if neccessary.
 * */
static void putEscapedByte(uint8_t data)
{
	// Escape special characters
	if ((data == ESC) || (data == DELIM))
	{
		Ringbuffer_put(&transmitBuffer,ESC);
	}
	Ringbuffer_put(&transmitBuffer, data);
}




/** The actual transmitter task function.
 * */
static void transmitterTask(void)
{
	do
	{
		uint8_t size = 0;
		uint8_t channel = 0;
		// Read size of the following message
		// (not counting size and channel byte)
	 	Pipe_read(&communicationTxPipe,&size);
		// Read first byte == channel
		Pipe_read(&communicationTxPipe,&channel);
		// Calculate initial checksum value == first byte to transmit
		uint8_t checkSum = 0;
		checkSum = (outPacketCount << 5) | (channel & 0x1F);
		// Wait until buffer is free and not in use by uart tx interrupt
		while (!Ringbuffer_empty(&transmitBuffer))
		{
			Task_waitCurrent(7);
		}
		// Put first byte into buffer
		putEscapedByte(checkSum);
		// Read following data bytes, recalculate checksum, put as escaped values
		// into the buffer
		uint8_t iterator = 0;
		do
		{
			uint8_t data = 0;
			Pipe_read(&communicationTxPipe,&data);
			putEscapedByte(data);
			checkSum ^= data;
			iterator++;
			if (iterator == TRANSMIT_BUFFER_SIZE - 3)
			{
				// The message is larger than the buffer. If this happens:
				// Damn it: You are stupid!
			}
		} while (iterator != size);
		// Write final checksum
		putEscapedByte(checkSum);
		// Put delimiter into buffer
		Ringbuffer_put(&transmitBuffer, DELIM);
		// Update statistics
		outPacketCount = (outPacketCount + 1) & 0x07;
		// Enable the uart, transmission will start shortly after
		readyToTransmit();
		// Wait per default to reduce transmission rate
		Task_waitCurrent(10);
	} while (1);
}




//-----------------------------------------------------------------------------
/** Get a byte from the receive buffer, waiting if neccessary.
 * 
 * This is not using a semaphore, as this seemed to be the crash
 * reason of the previous communication code.
 * */
static uint8_t receiveBuffer_get(void)
{
	uint8_t wait = 5;
	// Wait until there is data in the receiveBuffer
	while (Ringbuffer_empty(&receiveBuffer))
	{
		// Increase wait duration to reduce load for the waitQueue
		Task_waitCurrent(wait);
		wait += 5;
		if (wait >= 150)
		{
			wait = 100;
		}
	}
	// Get data from the buffer
	uint8_t data = Ringbuffer_get(&receiveBuffer);
	return data;
}




//-----------------------------------------------------------------------------
/** Receive handler, called from uart receive complete interrupt.
 * */
static void receiveHandler(void)
{
	uint8_t data = UDR0;
	// If reading task is to slow, this will result in garbage!
	// In this case, the receiveHandler will overwrite still pending data in the
	// buffer, leading to corrupt packets.
	Ringbuffer_put(&receiveBuffer, data);
}




/** Transmit handler, called from uart udr ready interrupt.
 * */
static void transmitHandler(void)
{
	// Check if there is any data left to transmit
	if (!Ringbuffer_empty(&transmitBuffer))
	{
		// If so, transmit it
		UDR0 = Ringbuffer_get(&transmitBuffer);
	}
	else
	{
		// Otherwise, disable this interrupt
		UCSR0B &= ~(1 << UDRIE0);
	}
}




/** Interrupt handlers
 * */
ISR(USART0_RX_vect)
{
	receiveHandler();
}
ISR(USART0_UDRE_vect)
{
	transmitHandler();
}




//-----------------------------------------------------------------------------
void Communication_writePacket(uint8_t channel, uint8_t * packet, uint8_t size)
{
	uint8_t iterator = 0;
	// Start write to ensure no other task is writing to the transmitter task
	// at the same time.
	Pipe_startWrite(&communicationTxPipe);
	// Write size of packet, excluding channel and size byte
	Pipe_write(&communicationTxPipe, size);
	Pipe_write(&communicationTxPipe, channel);
	while (iterator != size)
	{
		Pipe_write(&communicationTxPipe, packet[iterator]);
		iterator ++;
	}
	// Packet transmitted, the next task may use the transmitter.
	Pipe_endWrite(&communicationTxPipe);
}




uint8_t Communication_readPacket(uint8_t * buffer, uint8_t max)
{
	uint8_t checkSum = 0;
	uint8_t received = 0;
	do
	{
		// Read data from buffer
		uint8_t data = receiveBuffer_get();
		// If delimiter is found, assume complete packet transmitted
		if (data == DELIM)
		{
			// Ensure packet has been received without errors.
			if (checkSum == 0 && received >= 2)
			{
				// Isolate channel number
				buffer[0] &= 0x1F;
				return received;
			}
			else
			{
				// Checksum error
				return 0;
			}
		}
		// Noticed start of escaped byte pair
		if (data == ESC)
		{
			// Get next byte for the real data
			data = receiveBuffer_get();
		}
		// Write received byte into private buffer
		buffer[received] = data;
		// Update checksum
		checkSum ^= data;
		received++;
		// Protect agains buffer overflows
		if (received == max)
		{
			// Packet too large or error
			// Try to read (corrupt) package until next valid delimiter, in hope
			// to repair the communication link
			do
			{
				if (data == ESC)
				{
					data = receiveBuffer_get();
				}
				if (data == DELIM)
				{
					break;
				}
				data = receiveBuffer_get();
			} while(1);
			return 0;
		}
	} while(1);
}




void Communication_log(uint8_t level, const char* format, ...)
{
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

