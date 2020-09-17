#include "strings.h"

ByteString
AllocByteString(size_t size)
{
	ByteString s;
	s.buffer = (u8*)malloc(size);
	s.buffer_size = size;
	s.buffer[size-1] = 0;
	return s;
}

void
Dealloc(ByteString *s)
{
	free(s->buffer);
	s->buffer_size = 0;
}

Utf32String
AllocUtf32String(size_t char_count)
{
	Utf32String s;
	s.chars = (u32*)malloc(sizeof(u32)*char_count);
	s.char_count = char_count;
	return s;
}

void
Dealloc(Utf32String *s)
{
	free(s->chars);
	s->char_count = 0;
}

void
ResizeString(Utf32String *s, size_t char_count)
{
	s->chars = (u32*)realloc(s->chars, 4*char_count);
	s->char_count = char_count;
}



Utf32String
Utf8ToUtf32(ByteString utf8_string)
{
	Utf32String string_out = AllocUtf32String(4*utf8_string.buffer_size); // Upper bound is 4*utf8 size; we'll resize later

	u32 *utf32_p = string_out.chars;
	u8 *p = utf8_string.buffer;

	size_t c = 0; // character index into str[]

	while(c < utf8_string.buffer_size)
	{
		u8 first_byte = p[c];
		u32 char_code = 0;

		if(u8((first_byte >> 7) | utf8_mask_1byte) == utf8_mask_1byte)
		{
			char_code = first_byte;
			c += 1;
		}
		else if (u8((first_byte >> 5) | utf8_mask_2byte) == utf8_mask_2byte)
		{
			char_code += p[c+1] & 0b00111111;
			char_code += (p[c+0] & 0b00011111) << 6;
			c += 2;
		}
		else if (u8((first_byte >> 4) | utf8_mask_3byte) == utf8_mask_3byte)
		{
			char_code += p[c+2] & 0b00111111;
			char_code += (p[c+1] & 0b00111111) << 6;
			char_code += (p[c+0] & 0b00001111) << 12;
			c += 3;
		}
		else if(u8((first_byte >> 3) | utf8_mask_4byte) == utf8_mask_4byte)
		{
			char_code += p[c+3] & 0b00111111;
			char_code += (p[c+2] & 0b00111111) << 6;
			char_code += (p[c+1] & 0b00111111) << 12;
			char_code += (p[c+0] & 0b00000111) << 18;
			c += 4;
		}
		else
		{
			log("Encountered invalid utf-8 starting byte (decimal: %u)", char_code);
		}

		*utf32_p = char_code;
		utf32_p++;
	}

	ResizeString(&string_out, utf32_p-string_out.chars+1);
	string_out.chars[string_out.char_count-1] = 0;

	return string_out;
}

bool CompareCStrings(char *a, char *b) {
	int MAX_LENGTH = 100000;
	for(int i=0; i<MAX_LENGTH; i++) {
		if(a[i] == '\0' or b[i] == '\0') return true;
		if(a[i] != b[i]) return false;
	}

	// @TODO: Error report for reaching MAX_LENGTH
	return false;
}