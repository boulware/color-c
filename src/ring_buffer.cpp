#include "ring_buffer.h"

#include "math.h"

size_t
RingBufferBytesRemaining(volatile RingBuffer *buffer)
{
    return(buffer->size_in_bytes - buffer->next_index_to_write + buffer->next_index_to_read);
}

size_t
WriteBytesToRingBuffer(volatile RingBuffer *buffer, void *src, size_t byte_count)
{
    size_t data_start_index = buffer->next_index_to_write;

    size_t bytes_remaining_in_buffer = RingBufferBytesRemaining(buffer);
    size_t bytes_to_write = m::Min(byte_count, bytes_remaining_in_buffer);
    for(int i=0; i<bytes_to_write; ++i)
    {
        ((u8*)buffer->data)[(buffer->next_index_to_write + i) % buffer->size_in_bytes] = ((u8*)src)[i];
    }

    buffer->next_index_to_write = (buffer->next_index_to_write + bytes_to_write) % buffer->size_in_bytes;

    return data_start_index;
}

void
ReadBytesFromRingBuffer(volatile RingBuffer *buffer, void *dst, size_t byte_count)
{
    for(int i=0; i<byte_count; ++i)
    {
        ((u8*)dst)[i] = ((u8*)buffer->data)[(buffer->next_index_to_read + i) % buffer->size_in_bytes];
    }

    buffer->next_index_to_read = (buffer->next_index_to_read + byte_count) % buffer->size_in_bytes;
}