/** ringbuffer implementation
 * */
#include <stdint.h>
#include "util/ringbuffer.h"









//-----------------------------------------------------------------------------
void Ringbuffer_put(Ringbuffer * buffer, uint8_t data)
{
  *(buffer->write) = data;
  (buffer->write)++;
  if ((buffer->write) == (buffer->end))
  {
    buffer->write = buffer->start;
  }
}




uint8_t Ringbuffer_get(Ringbuffer * buffer)
{
  uint8_t data = *(buffer->read);
  (buffer->read)++;
  if ((buffer->read) == (buffer->end))
  {
    buffer->read = buffer->start;
  }
  return data;
}




void Ringbuffer_init(Ringbuffer * buffer, uint8_t memory[], uint8_t size)
{
  buffer->start = memory;
  buffer->end = buffer->start + size;
  buffer->write = buffer->start;
  buffer->read = buffer->start;
}

