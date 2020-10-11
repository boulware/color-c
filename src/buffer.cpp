#include "buffer.h"

Buffer BufferFromCString(const char *string)
{
    Buffer buffer;
    buffer.data = (char*)string;
    buffer.p = buffer.data;
    buffer.byte_count = StringLength(string);

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
        Log("Buffer *p (&data=%p) points to location outside buffer", buffer.data);
        return 0;
    }

    size_t remaining = buffer.data+buffer.byte_count-buffer.p;
    return(remaining);
}