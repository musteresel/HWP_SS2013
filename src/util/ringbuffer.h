#ifndef __UTIL__RINGBUFFER_H__
#define __UTIL__RINGBUFFER_H__ 1

typedef struct __Ringbuffer_t
{
	uint8_t * base;
	uint8_t  write;
	uint8_t  end;
	uint8_t  start;
} Ringbuffer;




//-----------------------------------------------------------------------------
extern void Ringbuffer_init(Ringbuffer *, uint8_t[], uint8_t);
extern void Ringbuffer_put(Ringbuffer *, uint8_t);
extern uint8_t Ringbuffer_get(Ringbuffer *);
extern uint8_t Ringbuffer_empty(Ringbuffer *);

#endif

