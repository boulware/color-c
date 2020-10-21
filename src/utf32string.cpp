#include "utf32string.h"

u32 &CharAt(Utf32String *string, u32 index)
{
	return string->data[index];
}

u32 *begin(Utf32String& string)
{
	return string.data;
}

u32 *end(Utf32String& string)
{
	return string.data+string.length;
}

bool
StringEmpty(Utf32String string)
{
	return(string.length == 0);
}

bool
StringFull(Utf32String string)
{
	return(string.length >= string.max_length);
}

// Returns false if string was already full.
bool
AppendChar(Utf32String *string, u32 appended_char)
{
	if(StringFull(*string)) return false;

	string->data[string->length++] = appended_char;
	return true;
}

bool
InsertChar(Utf32String *string, u32 inserted_char, int pos)
{
	if(StringFull(*string)) return false;
	if(pos < 0 or pos > string->length)
	{
		if(c::verbose_error_logging) Log(__FUNCTION__ "() received invalid pos for string (pos=%d)", pos);
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
DeleteChar(Utf32String *string, int pos)
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
DeleteRange(Utf32String *string, int start, int end)
{
	if(start >= end or StringEmpty(*string)) return false;
	if(start < 0 or start > string->length or end < 0 or end > string->length)
	{
		if(c::verbose_error_logging) Log(__FUNCTION__ "() received invalid indices for string (start=%d, end=%d)", start, end);
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


void
CopyFromCString(Utf32String *utf32_string, const char *c_string)
{
	const char *p = c_string;
	while(*p != '\0' and !StringFull(*utf32_string))
	{
		utf32_string->data[utf32_string->length++] = *p++;
	}
}

Utf32String
Utf32StringFromCString(Id<Arena> arena_id, const char *c_string)
{
	Utf32String string = {};
	string.length = StringLength(c_string);
	string.max_length = string.length;
	string.data = (u32*)AllocFromArena(arena_id, sizeof(u32)*string.length);

	return string;
}

bool
SubstringInString(Utf32String substring, Utf32String string)
{
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
			for(int j=0; j<string.length; j++)
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