/** ringbuffer implementation
 * */
#include <stdint.h>
#include "util/ringbuffer.h"









//-----------------------------------------------------------------------------
void Ringbuffer_put(Ringbuffer * buffer, uint8_t data)
{
  buffer->base[write] = data;
  buffer->write++;
  if (buffer->write == buffer->end)
  {
    buffer->write = 0;
  }
}




uint8_t Ringbuffer_get(Ringbuffer * buffer)
{
  uint8_t data = buffer->base[read];
  buffer->read++;
  if (buffer->read == buffer->end)
  {
    buffer->read = 0;
  }
  return data;
}




void Ringbuffer_init(Ringbuffer * buffer, uint8_t memory[], uint8_t size)
{
  buffer->base = memory;
  buffer->end = size;
  buffer->write = 0;
  buffer->read = 0;
}


uint8_t Ringbuffer_empty(Ringbuffer * buffer)
{
	return (buffer->write == buffer->read);
}
