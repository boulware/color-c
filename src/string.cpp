#include "string.h"

#include "text_parsing.h"

char &CharAt(String *string, u32 index)
{
	return string->data[index];
}

char *begin(String& string)
{
	return string.data;
}

char *end(String& string)
{
	return string.data+string.length;
}

bool
StringEmpty(String string)
{
	return(string.length == 0);
}

bool
StringFull(String string)
{
	return(string.length >= string.max_length);
}

// Returns false if string was already full.
bool
AppendChar(String *string, char appended_char)
{
	if(StringFull(*string)) return false;

	string->data[string->length++] = appended_char;
	return true;
}

bool
InsertChar(String *string, char inserted_char, int pos)
{
	if(StringFull(*string)) return false;
	if(pos < 0 or pos > string->length)
	{
		if(c::verbose_error_logging) log(__FUNCTION__ "() received invalid pos for string (pos=%d)", pos);
		return false;
	}

	int char_count_to_copy = string->length - pos;
	for(int i=string->length; i>pos; --i)
	{
		CharAt(string, i) = CharAt(string, i-1);
	}

	CharAt(string, pos) = inserted_char;

	++string->length;
	return true;
}

// Deletes char from string. If pos<0, it will assume the char at the end of the string.
// Returns false if string was already empty.
bool
DeleteChar(String *string, int pos)
{
	if(StringEmpty(*string)) return false;

	if(pos >= 0)
	{
		for(int i=pos+1; i<string->length; i++)
		{
			CharAt(string, i-1) = CharAt(string, i);
		}
	}

	--string->length;
	return true;
}

// End is non-inclusive. e.g., if start==end, no chars are deleted.
// Range [0,1] deletes only the first character of the string.
// The start and end indices are not indicators for the character index,
// but more like indicators for where an imaginary cursor would be
// (where cursor=0 means it's just before the first character)
bool
DeleteRange(String *string, int start, int end)
{
	if(start >= end or StringEmpty(*string)) return false;
	if(start < 0 or start > string->length or end < 0 or end > string->length)
	{
		if(c::verbose_error_logging) log(__FUNCTION__ "() received invalid indices for string (start=%d, end=%d)", start, end);
		return false;
	}

	// char_count_to_copy is to make sure we're copying the correct number of characters.
	// It's difficult to succinctly explain the logic of this calculation, but if you want to see
	// why the number of characters to copy might differ, consider e.g., 5-character string where
	// you want to delete on [0,2] vs. where you want to delete [0,4]
	int char_count_to_copy = m::Max(string->length - end, end - start);
	for(int i=0; i<char_count_to_copy; i++)
	{
		CharAt(string, start+i) = CharAt(string, end+i);
	}

	string->length -= (end-start);
	return true;
}

bool
AppendCString(String *string, const char *c_string, ...)
{
	if(StringFull(*string)) return false;

	char *formatted_string;
	mFormatString(formatted_string, c_string);

	bool entire_string_appended = false;
	size_t max_chars_to_append = string->max_length - string->length;
	for(int i=0; i<max_chars_to_append; i++)
	{
		if(formatted_string[i] == '\0')
		{
			entire_string_appended = true;
			break;
		}

		string->data[string->length++] = formatted_string[i];
	}

	if(!entire_string_appended and formatted_string[max_chars_to_append] == '\0')
	{
		entire_string_appended = true;
	}

	return entire_string_appended;
}

String
StringFromCString(const char *c_string, Arena *arena)
{
	String string = {};
	string.length = StringLength(c_string);
	string.max_length = string.length;
	string.data = (char*)AllocFromArena(arena, sizeof(char)*string.length);
	for(int i=0; i<string.length; i++)
	{
		CharAt(&string, i) = c_string[i];
	}

	return string;
}

bool
SubstringInString(String substring, String string)
{
	TIMED_BLOCK;

	if(substring.length == 0) return true;
	if(string.length == 0 and substring.length == 0) return true;
	if(substring.length > string.length) return false;

	// Location of the beginning of the previous substring search.
	int sub_start = -1;

	// The last index in string where a valid substring could start (based on length)
	int last_valid_substring_start = string.length - substring.length;

	for(int i=0; i<=last_valid_substring_start; i++)
	{ // Iterate over each char in string.
		if(CharAt(&string, i) == CharAt(&substring, 0))
		{ // The potential start of a substring
			sub_start = i;
			bool matches = true;
			for(int j=0; j<substring.length; j++)
			{ // Check consecutive chars of string and substring to be equal over the entire length of substring
				if(CharAt(&string, i+j) != CharAt(&substring, j))
				{
					// A non-equal character was found, so the substring doesn't match. We break,
					// and go back to looking for first-char matches in the string.
					matches = false;
					break;
				}
			}

			if(matches) return true;
		}
	}

	return false;
}

void
CopyFromCString(String *string, const char *c_string)
{
	const char *p = c_string;
	while(*p != '\0' and !StringFull(*string))
	{
		string->data[string->length++] = *p++;
	}
}

// Interprets the [string], starting at [index], as a utf8 byte sequence and stores the
// interpreted value as a utf-32 character in [utf32_char]
// Returns the number of bytes interpreted.
int
Utf8ToUtf32(String string, int index, u32 *utf32_char)
{
	int bytes_remaining = string.length - index;
	if(bytes_remaining <= 0) return false;

	//u8 first_byte = CharAt(string, index);
	u8 first_byte = CharAt(&string, index);
	u32 char_code = 0;
	int byte_count_interpreted = 0;

	if(u8((first_byte >> 7) | utf8_mask_1byte) == utf8_mask_1byte)
	{
		char_code = first_byte;
		byte_count_interpreted = 1;
	}
	else if (u8((first_byte >> 5) | utf8_mask_2byte) == utf8_mask_2byte and bytes_remaining >= 2)
	{
		char_code += (CharAt(&string, index+1) & 0b00111111) << 0;
		char_code += (CharAt(&string, index+0) & 0b00011111) << 6;
		byte_count_interpreted = 2;
	}
	else if (u8((first_byte >> 4) | utf8_mask_3byte) == utf8_mask_3byte and bytes_remaining >= 3)
	{
		char_code += (CharAt(&string, index+2) & 0b00111111) << 0;
		char_code += (CharAt(&string, index+1) & 0b00111111) << 6;
		char_code += (CharAt(&string, index+0) & 0b00001111) << 12;
		byte_count_interpreted = 3;
	}
	else if(u8((first_byte >> 3) | utf8_mask_4byte) == utf8_mask_4byte and bytes_remaining >= 4)
	{
		char_code += (CharAt(&string, index+3) & 0b00111111) << 0;
		char_code += (CharAt(&string, index+2) & 0b00111111) << 6;
		char_code += (CharAt(&string, index+1) & 0b00111111) << 12;
		char_code += (CharAt(&string, index+0) & 0b00000111) << 18;
		byte_count_interpreted = 4;
	}
	else
	{
		//log("Encountered invalid utf-8 starting byte, or sequence of remaining bytes was not long enough.", char_code);
		return false;
	}

	*utf32_char = char_code;
	return byte_count_interpreted;
}

String
CopyString(String src, Arena *arena)
{
	String copied_string = {};
	copied_string.length = src.length;
	copied_string.max_length = src.max_length;
	copied_string.data = (char*)AllocFromArena(arena, src.max_length);
	for(int i=0; i<src.length; i++)
	{
		CharAt(&copied_string, i) = CharAt(&src, i);
	}

	return copied_string;
}

// Default argument for arena is memory::per_frame_arena
String
LowerCase(String string, Arena *arena)
{
	String lowered_string = CopyString(string, arena);
	for(int i=0; i<lowered_string.length; i++)
	{
		char &c = CharAt(&lowered_string, i);
		if(c >= 'A' and c <= 'Z')
		{
			c += ('a' - 'A');
		}
	}

	return lowered_string;
}