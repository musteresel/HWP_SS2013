/** Message queue implementation
 * */
#ifndef __KERNEL__MESSAGE_H__
#define __KERNEL__MESSAGE_H__ 1
#include "kernel/semaphore.h"
#include "kernel/task.h"
#include <stdint.h>




//-----------------------------------------------------------------------------
typedef struct __Message_t
{
	Task * from;
	uint8_t * buffer;
	uint8_t size;
	struct __Message_t * next;
} Message;




typedef struct __MessageBox_t
{
	Message * next;
	Message * last;
	Semaphore emptyCount;
	Semaphore fillCount;
	Semaphore lock;
} MessageBox;




//-----------------------------------------------------------------------------
/** Init a message box
 * */
extern void Message_initBox(MessageBox *, uint8_t);
/** Read a message from a box, blocking
 * */
extern Message * Message_receive(MessageBox *);
/** Send a message to a box, blocking
 * */
extern void Message_send(MessageBox *, uint8_t *, uint8_t);
/** Free a message after use
 * */
extern void Message_free(Message *);




#endif

