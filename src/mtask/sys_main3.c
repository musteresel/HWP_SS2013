#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include "util/attribute.h"
#include "kernel/task.h"
#include "device/uart.h"
#include "kernel/pipe.h"
#include "util/communication.h"


//-----------------------------------------------------------------------------
#ifdef DEBUG_SIMAVR
#include "avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega1280");
const struct avr_mmcu_vcd_trace_t _mytrace[]  _MMCU_ = {
	{ AVR_MCU_VCD_SYMBOL("UDR0"), .what = (void*)&UDR0, },
	{ AVR_MCU_VCD_SYMBOL("UDRE0"), .mask = (1 << UDRE0), .what = (void*)&UCSR0A, },
	{ AVR_MCU_VCD_SYMBOL("UDRIE0"), .mask = (1 << UDRIE0), .what = (void*)&UCSR0B, },
	{ AVR_MCU_VCD_SYMBOL("GI"), .mask = (1 << 7), .what = (void*)&SREG, },
	{ AVR_MCU_VCD_SYMBOL("GPIOR0"), .what = (void*)&GPIOR0, },
};
#endif




//-----------------------------------------------------------------------------
ATTRIBUTE( naked ) void main(void)
{
	GPIOR1 = SIMAVR_CMD_VCD_START_TRACE;
	uint8_t item = 42;
	Multitasking_init();
}


ATTRIBUTE( noinline ) void BREAKPOINT(void)
{
	asm volatile ("nop" ::: "memory");
}

volatile uint8_t a = 0;
//-----------------------------------------------------------------------------
TASK_STATIC(producer,1,producerFct,200,1);
void producerFct(void)
{
	uint8_t item = 20;
	Task_waitCurrent(1000);
	do
	{
		Communication_log(3,"Alive %u (%u)\n", item, a);
		item = item + 1;
		Task_waitCurrent(3000);
		if (item == 25)
		{
			BREAKPOINT();
			cli();
			UCSR0A |= (1 << RXC0);
			sei();
		}
	} while (1);
}



AVR_MCU_SIMAVR_COMMAND(&GPIOR1);

extern uint8_t uart0_rxPipeBuffer[];

TASK_STATIC(commander,2,commanderFct,200,1);
void commanderFct(void)
{
	uint8_t buffer[50];
	//GPIOR1 = SIMAVR_CMD_UART_LOOPBACK;
	Task_waitCurrent(500);
	do
	{
		uint8_t item;
		Task_waitCurrent(3000);
		GPIOR0 = 20;
		Pipe_read(&uart0_rxPipe, &item);
		Communication_log(1,"Data: %u\n", uart0_rxPipe.emptyCount.count);
		//Communication_readPacket((uint8_t*)&(buffer[0]),50);
		continue;
		Communication_log(3,"Got packet");
		if (buffer[0] == 2)
		{
			Communication_log(3,"Got drive command!");
		}
		//
	} while (1);
}







void UartTransmitter(void)
{
	do
	{
		uint8_t item = 0;
		Pipe_read(&uartTransmitPipe, &item);
		cli();
		Ringbuffer_put(&uartTransmitBuffer, item);
		Uart0_readyToTransmit();
		sei();
	} while (1);
}
	

void Uart_txHandler(Uart * uart)
{
	if (!Ringbuffer_empty(&(uart->transmitBuffer)))
	{
		*(uart->UDR) = Ringbuffer_get(&(uart->transmitBuffer));
	}
	else
	{
		*(uart->UCSR0B) &= ~(uart->udrInterruptMask);
	}
}
