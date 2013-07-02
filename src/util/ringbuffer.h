#ifndef __UTIL__RINGBUFFER_H__
#define __UTIL__RINGBUFFER_H__ 1

typedef struct __Ringbuffer_t
{
	uint8_t * read;
	uint8_t * write;
	uint8_t * end;
	uint8_t * start;
} Ringbuffer;




//-----------------------------------------------------------------------------
extern void Ringbuffer_init(Ringbuffer *, uint8_t[], uint8_t);
extern void Ringbuffer_put(Ringbuffer *, uint8_t);
extern uint8_t Ringbuffer_get(Ringbuffer *);


#endif

