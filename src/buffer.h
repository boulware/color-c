#ifndef BUFFER_H
#define BUFFER_H

struct Buffer
{
    char *data;
    char *p; // current position within *data
    size_t byte_count; // Generally, this will hide the fact that Buffer is null-terminated.
                       // byte_count should represent the number of bytes of 'real' data.
                       // As such, malloc()s should ask for byte_count+1 bytes.
};

Buffer BufferFromCString(const char *string);
void FreeBuffer(Buffer *buffer);
size_t BufferBytesRemaining(Buffer buffer);

#endif