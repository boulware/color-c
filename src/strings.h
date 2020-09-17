#ifndef STRINGS_H
#define STRINGS_H

#include "types.h"

// const u8 utf8_mask_1byte = 0b0;
// const u8 utf8_mask_2byte = 0b110;
// const u8 utf8_mask_3byte = 0b1110;
// const u8 utf8_mask_4byte = 0b11110;

struct ByteString
{
	u8 *buffer;
	size_t buffer_size;
};

ByteString
AllocByteString(size_t size);

void
Dealloc(ByteString *s);

struct Utf32String
{
	u32 *chars;
	size_t char_count;
};

Utf32String
AllocUtf32String(size_t char_count);

void
Dealloc(Utf32String *s);

void
ResizeString(Utf32String *s, size_t size);

bool
CompareCStrings(char *a, char *b);

#endif