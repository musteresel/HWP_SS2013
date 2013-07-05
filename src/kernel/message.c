#include <stdint.h>
#include "kernel/semaphore.h"
#include "kernel/message.h"
#include "kernel/memory.h"




//-----------------------------------------------------------------------------
void Message_initBox(MessageBox * box, uint8_t count)
{
	box->next = 0;
	box->last = 0;
	Semaphore_init(&(box->emptyCount),count);
	Semaphore_init(&(box->fillCount),0);
	Semaphore_init(&(box->lock),1);
}




//-----------------------------------------------------------------------------
Message * createMessage(uint8_t size)
{
	uint8_t * messageAndBuffer = (uint8_t *) Memory_alloc(sizeof(Message) + size);
	if (messageAndBuffer)
	{
		asm volatile ("nop");
	}
	Message * message = (Message *)messageAndBuffer;
	message->buffer = messageAndBuffer + sizeof(Message);
	message->size = size;
	return message;
}




void destroyMessage(Message * message)
{
	uint8_t * messageAndBuffer = (uint8_t *)message;
	Memory_free(messageAndBuffer);
}




void memcpy(uint8_t * target, uint8_t * source, uint8_t count)
{
	while (count--)
	{
		*target++ = *source++;
	}
}




//-----------------------------------------------------------------------------
Message * Message_receive(MessageBox * box)
{
	Semaphore_wait(&(box->fillCount));
	Semaphore_wait(&(box->lock));
	Message * message = box->next;
	box->next = box->next->next;
	if (!box->next) box->last = 0;
	Semaphore_signal(&(box->lock));
	Semaphore_signal(&(box->emptyCount));
	return message;
}




void Message_send(MessageBox * to, uint8_t * buffer, uint8_t size)
{
	Semaphore_wait(&(to->emptyCount));
	Semaphore_wait(&(to->lock));
	Message * message = createMessage(size);
	message->from = Task_getCurrent();
	memcpy(message->buffer, buffer, size);
	if (to->last)
	{
		to->last->next = message;
	}
	else
	{
		to->last = to->next = message;
	}
	message->next = 0;
	Semaphore_signal(&(to->lock));
	Semaphore_signal(&(to->fillCount));
}




void Message_free(Message * message)
{
	destroyMessage(message);
}

