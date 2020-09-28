#ifndef UTF32STRING_H
#define UTF32STRING_H

// This is a comment
//#define Introspect

#include "vec.h"
#include "memory.h"

Introspect
struct Utf32String
{
	int length;
	int max_length;
	u32 *data;
	Vec2f pos;
};

u32 &CharAt(Utf32String *string, u32 index);

u32 *begin(Utf32String& string);
u32 *end(Utf32String& string);

bool StringEmpty(Utf32String string);
bool StringFull(Utf32String string);

bool AppendChar(Utf32String *string, u32 appended_char);
bool InsertChar(Utf32String *string, u32 inserted_char, int pos);
bool DeleteChar(Utf32String *string, int pos);
bool DeleteRange(Utf32String *string, int start, int end);

bool SubstringInString(Utf32String substring, Utf32String string);

void CopyFromCString(Utf32String *utf32_string, const char *c_string);
Utf32String Utf32StringFromCString(Arena *arena, const char *c_string);

#endif