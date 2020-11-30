#include "buffer.h"

Buffer
BufferFromCString(const char *string)
{
    Buffer buffer;
    buffer.data = (char*)string;
    buffer.p = buffer.data;
    buffer.byte_count = StringLength(string);

    return buffer;
}

Buffer
BufferFromString(String string)
{
    Buffer buffer;
    buffer.data = string.data;
    buffer.p = buffer.data;
    buffer.byte_count = string.length;

    return buffer;
}

void
FreeBuffer(Buffer *buffer)
{
    free(buffer->data);
    buffer = {};
}

size_t
BufferBytesRemaining(Buffer buffer)
{
    //         |||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
    //         ....................s.............p.....................e
    // s    -------------------->
    // s+sz -------------------------------------------------------->
    // p    ---------------------------------->
    if(buffer.p < buffer.data or buffer.p > buffer.data+buffer.byte_count)
    {
        if(buffer.p > buffer.data+buffer.byte_count+1)
        { // [p] must point within the buffer or at the first byte outside the buffer to be valid.
            Log("Buffer *p (&data=%p) points to invalid location outside buffer", buffer.data);
        }

        return 0;
    }

    size_t remaining = buffer.data+buffer.byte_count-buffer.p;
    return(remaining);
}