#include "csv.h"

u32
NextUtf32Char(Buffer *buffer)
{
	u32 bytes_remaining = BufferBytesRemaining(*buffer);
	if(bytes_remaining == 0) return 0;

	u8 *p = buffer->data + buffer->pos;
	u8 first_byte = *p;
	u32 char_code = 0;

	if(u8((first_byte >> 7) | utf8_mask_1byte) == utf8_mask_1byte)
	{
		char_code = first_byte;

		buffer->pos += 1;
	}
	else if (u8((first_byte >> 5) | utf8_mask_2byte) == utf8_mask_2byte)
	{
		if(bytes_remaining < 2) return 0;
		char_code += (p[1] & 0b00111111) << 0;
		char_code += (p[0] & 0b00011111) << 6;

		buffer->pos += 2;
	}
	else if (u8((first_byte >> 4) | utf8_mask_3byte) == utf8_mask_3byte)
	{
		if(bytes_remaining < 3) return 0;
		char_code += (p[2] & 0b00111111) << 0;
		char_code += (p[1] & 0b00111111) << 6;
		char_code += (p[0] & 0b00001111) << 12;

		buffer->pos += 3;
	}
	else if(u8((first_byte >> 3) | utf8_mask_4byte) == utf8_mask_4byte)
	{
		if(bytes_remaining < 4) return 0;
		char_code += (p[3] & 0b00111111) << 0;
		char_code += (p[2] & 0b00111111) << 6;
		char_code += (p[1] & 0b00111111) << 12;
		char_code += (p[0] & 0b00000111) << 18;

		buffer->pos += 4;
	}
	else
	{
		log("Encountered invalid utf-8 initial byte (decimal: %u)", char_code);
	}

	return char_code;
}

// Utf32String
// NextToken(Buffer *buffer, const char *delim, const char *skip)
// {
// 	Utf32String string = AllocUtf32String(16);
// 	int i = 0;

// 	u32 cur_char = NextUtf32Char(file);

// 	// Trim whitespace at beginning of token.
// 	while(IsWhitespace(cur_char) == true)
// 	{
// 		cur_char = NextUtf32Char(file);
// 	}

// 	while(i < string.char_count)
// 	{
// 		// Iterate string until we find a comma, which indicates the end of a token.
// 		string.chars[i] = cur_char;
// 		if(cur_char == ',') break;

// 		i += 1;
// 		cur_char = NextUtf32Char(file);
// 	}

// 	if(i == string.char_count-1)
// 	{
// 		log("NextToken(CsvFile *) default token buffer was either just large enough or not large enough.");
// 	}

// 	// Find last non-whitespace character in string so we can trim it.
// 	// @todo: I question the stability of this for edge cases like empty or all-whitespace strings
// 	while((string.chars[i] == ',' or IsWhitespace(string.chars[i])) and i >= 0)
// 	{
// 		i -= 1;
// 	}

// 	ResizeString(&string, i+1);

// 	return string;
// }

// void
// CloseCsvFile(CsvFile *file)
// {
// 	if(file->buffer != nullptr) free(file->buffer);
// 	file->buffer_size = 0;
// 	file->buffer = nullptr;
// 	file->pos = nullptr;
// }