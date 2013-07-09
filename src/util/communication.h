#ifndef __UTIL__COMMUNICATION_H__
#define __UTIL__COMMUNICATION_H__ 1
#include <stdint.h>
#include "kernel/pipe.h"

#define COMMUNICATION_INFO 3
#define COMMUNICATION_WARNING 1

extern uint8_t Communication_readPacket(uint8_t *, uint8_t);
extern void Communication_writePacket(uint8_t, uint8_t *, uint8_t);
extern void Communication_log(uint8_t, const char *, ...);

extern Pipe uart0_txPipe;
extern Pipe uart0_rxPipe;


#endif

