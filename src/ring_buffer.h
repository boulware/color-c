#ifndef RING_BUFFER_H
#define RING_BUFFER_H

// @todo: Changing this so that any write we make will produce a contiguous series of bytes
//        could be useful, so that we can just pass in a pointer to the ring buffer data
//        instead of having to read it out into a temp buffer.
//        ... There's a virtual memory trick where you map the same page twice so you can read past the
//            end of where the first page ends and it will "wrap" back into the beginning of the page.
struct RingBuffer
{
    size_t next_index_to_read;
    size_t next_index_to_write;

    size_t size_in_bytes;
    void *data;
};

size_t RingBufferBytesRemaining(volatile RingBuffer *buffer);
size_t WriteBytesToRingBuffer(volatile RingBuffer *buffer, void *src, size_t byte_count);
void ReadBytesFromRingBuffer(volatile RingBuffer *buffer, void *dst, size_t byte_count);

size_t RingBufferBytesRemaining(RingBuffer *buffer);
size_t WriteBytesToRingBuffer(RingBuffer *buffer, void *src, size_t byte_count);
void ReadBytesFromRingBuffer(RingBuffer *buffer, void *dst, size_t byte_count);

#endif