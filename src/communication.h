/** Communication subsystem
 * */
#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__ 1
#include <stdint.h>




//-----------------------------------------------------------------------------
/** Log levels
 * */
#define COMMUNICATION_WARNING 1
#define COMMUNICATION_FINE 3




//-----------------------------------------------------------------------------
/** Read a packet from the receive buffer. CAUTION!
 *
 * This function may only be used from a single task, otherwise
 * it must be guared by a semaphore within all task accessing it!
 * This is not blocking, but waiting, if no data is currently available!
 *
 * Returns the size of the packet.
 * The first byte of the packet is the packet channel.
 * */
extern uint8_t Communication_readPacket(uint8_t *, uint8_t);




/** Write a packet to the transmitter task.
 * */
extern void Communication_writePacket(uint8_t, uint8_t *, uint8_t);




/** Assemble a log packet and send it. Tasks using this function need more than
 * 80 bytes of free stack space!
 * */
extern void Communication_log(uint8_t, const char *, ...);




//-----------------------------------------------------------------------------
#endif

