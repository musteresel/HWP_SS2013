#ifndef __DEVICE__UART_H__
#define __DEVICE__UART_H__ 1
#include <stdint.h>


extern void Uart_init(void);

#define __DEVICE__UART_H__decl(n) \
	extern void Uart ## n ## _transmit(uint8_t data); \
	extern uint8_t Uart ## n ## _receive(void);


__DEVICE__UART_H__decl(0);
__DEVICE__UART_H__decl(1);
__DEVICE__UART_H__decl(2);
__DEVICE__UART_H__decl(3);

#endif

