#include "text_parsing.h"

size_t StringLength(const char *s)
{
	size_t i = 0;
	while(s[i] != 0) ++i;
	return i;
}

bool
CompareStrings(const char *a, const char *b)
{
	if(*a == '\0' or *b == '\0')
	{
		// For now, we let empty strings be equal to each other,
		// but we don't allow this function to early out for empty strings,
		// because then "" == any arbitrary string
		if(*a == *b)
		{
			return true;
		}
		else return false;
	}
	// Compare strings, but if one is null-terminated before the other,
	// they will still be considered equal as long as all other bytes before
	// the null byte matched. See CompareStringsStrict() for a more strict comparison.

	// @bug: There's a buffer over-read bug here if either string either isn't null-terminated,
	// or doesn't own the null-terminator as part of its mapped memory. But if we're using C strings,
	// we should just always null-terminate everything anyway.

	size_t i = 0;
	while(a[i] != '\0' and b[i] != '\0')
	{
		if(a[i] != b[i]) {
			return false;
		}
		i += 1;
	}

	return true;
}

StringBuffer
CreateStringBuffer(const char *string)
{
	StringBuffer buffer;
	buffer.data = string;
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
	// 		|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	// 		....................s.............p.....................e
	// s	-------------------->
	// s+sz -------------------------------------------------------->
	// p    ---------------------------------->
	if(buffer.p < buffer.data or buffer.p > buffer.data+buffer.byte_count)
	{
		//log("Buffer *p (&data=%p) points to location outside buffer", buffer.data);
		return 0;
	}

	size_t remaining = buffer.data+buffer.byte_count-buffer.p;
	return(remaining);
}

size_t
BufferBytesRemaining(StringBuffer buffer)
{
	// 		|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
	// 		....................s.............p.....................e
	// s	-------------------->
	// s+sz -------------------------------------------------------->
	// p    ---------------------------------->
	if(buffer.p < buffer.data or buffer.p > buffer.data+buffer.byte_count)
	{
		//log("Buffer *p (&data=%p) points to location outside buffer", buffer.data);
		return 0;
	}

	size_t remaining = buffer.data+buffer.byte_count-buffer.p;
	return(remaining);
}

bool CharInString(char target, const char *string)
{
	for(int i=0; i<StringLength(string); i++)
	{
		if(string[i] == target) return true;
	}

	return false;
}

bool
IsWhitespace(char c)
{
	return(c == ' ' or
	   c == '\t' or
	   c == '\n' or
	   c == '\v' or
	   c == '\f' or
	   c == '\r');
}

bool
IsAlpha(char c)
{
	return((c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z'));
}

bool
IsDigit(char c)
{
	return(c >= '0' and c <= '9');
}

s32
CharToInt(char c)
{
	if(!IsDigit(c))
	{
		//log("CharToInt() received non-digit character.");
		return 0;
	}
	else
	{
		return int(c)-int('0');
	}
}

// Skips all characters in char array *skip, and returns true if any characters were skipped
bool
SkipChars(Buffer *buffer, const char *skip)
{
	bool skipped_at_least_once = false;
	size_t bytes_remaining = BufferBytesRemaining(*buffer);
	for(int _=0; _<bytes_remaining; _++)
	{
		if(CharInString(*buffer->p, skip))
		{
			skipped_at_least_once = true;
			buffer->p++;
		}
		else break;
	}

	return skipped_at_least_once;
}

// Seek to char c.
// If found, return true. Buffer position will be set to the location of the char c.
// If not found, return false. Buffer position will be reset to its location before the call.
bool
SeekChar(Buffer *buffer, char c)
{
	char *initial = buffer->p;

	int _ = 0;
	size_t bytes_remaining = BufferBytesRemaining(*buffer);
	for(_=0; _<bytes_remaining; _++)
	{
		if(*buffer->p == c) {
			return true;
		}
		else {
			buffer->p++;
		}
	}

	// We got to the end of the buffer and never found the target char.
	buffer->p = initial;
	return false;
}

bool
SeekAfterChar(Buffer *buffer, char c)
{
	if(SeekChar(buffer, c))
	{
		buffer->p++;
		return true;
	}
	else return false;
}

bool
SeekBeforeChar(Buffer *buffer, char c)
{
	if(SeekChar(buffer, c))
	{
		if(buffer->p > buffer->data)
		{
			buffer->p--;
		}
		return true;
	}
	else return false;
}

bool
ParseNextAsS32(Buffer *buffer, s32 *value)
{
	bool valid_token = true;
	char *initial = buffer->p;

	s8 multiplier = 1;
	s32 result = 0;

	SkipChars(buffer, c::whitespace);
	size_t bytes_remaining = BufferBytesRemaining(*buffer);
	bool start_of_token_found = false;
	bool at_least_one_digit_found = false;
	for(int _=0; _<bytes_remaining; _++)
	{
		if(IsDigit(*buffer->p))
		{
			if(!start_of_token_found) start_of_token_found = true;
			at_least_one_digit_found = true;

			result *= 10;
			result += CharToInt(*buffer->p);
		}
		else if(*buffer->p == '-')
		{
			if(!start_of_token_found)
			{
				multiplier = -1;
				start_of_token_found = true;
			}
			else break;
		}
		else if(*buffer->p == '+')
		{
			if(!start_of_token_found)
			{
				multiplier = +1;
				start_of_token_found = true;
			}
			else break;
		}
		else
		{
			//log("ParseNextAsS32() found illegal character (%c)", *buffer->p);
			if(start_of_token_found and at_least_one_digit_found)
			{
				// We've found the start of the token and interpreted at least one digit, so this s32 is valid.
				// This will be considered the end of the s32. We'll break out and write to the output value.
				break;
			}
			else
			{
				// The characters found in the buffer didn't match a valid s32 interpetation, so we'll
				// reset the buffer to its initial position and not write to the output value.
				valid_token = false;
				break;
			}
		}

		buffer->p++;
	}

	if(valid_token)
	{
		*value = multiplier*result;
	}
	else
	{
		buffer->p = initial;
	}

	return valid_token;
}

char
FirstTokenChar(Token token)
{
	if(token.length > 0)
	{
		return *(token.start);
	}
	else
	{
		return '\0';
	}
}

char
LastTokenChar(Token token)
{
	if(token.length > 0)
	{
		return *(token.start + token.length - 1);
	}
	else
	{
		return '\0';
	}
}

bool
CheckNextChar(Buffer *buffer, char c)
{
	size_t bytes_remaining = BufferBytesRemaining(*buffer);
	for(size_t i=0; i<bytes_remaining; i++)
	{
		char cur_char = *(buffer->p+i);

		if(cur_char == c) return true;
		else if(IsWhitespace(cur_char)) continue;
		else return false;
	}

	// Every remaining byte was whitespace and not [c].
	return false;
}

bool
ConfirmNextChar(Buffer *buffer, char c)
{
	char *initial = buffer->p;

	// If the char we're confirming is whitespace, we check for whitespace at current buffer pos and return...
	if(CharInString(c, c::whitespace) and CharInString(*buffer->p, c::whitespace)) return true;

	// ... otherwise, we skip all the beginning whitespace
	SkipChars(buffer, c::whitespace);
	if(*buffer->p++ == c)
	{
		return true;
	}
	else
	{
		buffer->p = initial;
		return false;
	}
}

bool
SeekNextLineThatBeginsWith(Buffer *buffer, const char *start_string)
{
	size_t start_string_length = StringLength(start_string);
	while((buffer->data == buffer->p or SeekAfterChar(buffer, '\n')) and BufferBytesRemaining(*buffer) >= start_string_length)
	{
		if(CompareStrings(buffer->p, start_string))
		{
			return true;
		}
		if(buffer->data == buffer->p) ++buffer->p;
	}

	// We fail because 1) Never found a new line, 2) Found a newline but there were not enough remaining bytes to be
	// able to match the target string, or 3) Found a newline but the string at the beginning of that
	// line didn't match the target string.
	return false;
}

bool
SeekAfterNextLineThatBeginsWith(Buffer *buffer, const char *start_string)
{
	size_t start_string_length = StringLength(start_string);
	while((buffer->data == buffer->p or SeekAfterChar(buffer, '\n')) and BufferBytesRemaining(*buffer) >= start_string_length)
	{
		if(CompareStrings(buffer->p, start_string))
		{
			buffer->p += start_string_length-1;
			return true;
		}
	}

	// We fail because 1) Never found a new line, 2) Found a newline but there were not enough remaining bytes to be
	// able to match the target string, or 3) Found a newline but the string at the beginning of that
	// line didn't match the target string.
	return false;
}

// Fetch next token, left- and right-trimming any char in trim, and setting the buffer position
// to immediately after the token (including skipping right-side trim characters)
// Token
// NextToken(Buffer *buffer, const char *trim = c::whitespace)
// {
// 	Token token = {};

// 	// Skip trim characters at beginning of token
// 	size_t bytes_remaining = BufferBytesRemaining(*buffer);
// 	for(size_t _=0; _<bytes_remaining; _++)
// 	{
// 		if(CharInString(*buffer->p, trim))
// 		{
// 			buffer->p++;
// 		}
// 		else break;
// 	}

// 	token.start = buffer->p;

// 	bytes_remaining = BufferBytesRemaining(*buffer);
// 	for(size_t _=0; _<bytes_remaining; _++)
// 	{
// 		if(!CharInString(*buffer->p, trim))
// 		{
// 			buffer->p++;
// 		}
// 		else break;
// 	}

// 	token.length = buffer->p - token.start;

// 	return token;
// }

// Try to parse next token as double-quoted string. Return true on success and the token without double-quotes
// will be stored in *token. Return false if next token is not a double-quoted string and don't advance buffer.
bool
NextTokenAsDoubleQuotedString(Buffer *buffer, Token *token)
{
	char *initial = buffer->p;
	bool is_valid_double_quoted_string = false;

	Token temp_token;
	if(!CheckNextChar(buffer, '\"'))
	{
		return false;
	}

	if(SeekAfterChar(buffer, '\"'))
	{
		temp_token.start = buffer->p;
		if(SeekAfterChar(buffer, '\"'))
		{
			// We are currently pointing *after* the closing quote,
			// so we subtract two from the length, one for the char
			// after closing quote and one for the quote itself.
			temp_token.length = buffer->p - temp_token.start - 1;
			is_valid_double_quoted_string = true;
		}
	}

	if(is_valid_double_quoted_string)
	{
		*token = temp_token;
	}
	else
	{
		buffer->p = initial;
	}

	return is_valid_double_quoted_string;
}

// Returns false if no bytes remain in buffer or invalid utf-8 bytes are encountered
bool
NextAsUtf32Char(Buffer *buffer, u32 *utf32_char)
{
	//char *initial = buffer->p;

	size_t bytes_remaining = BufferBytesRemaining(*buffer);
	if(bytes_remaining <= 0) return false;

	u8 first_byte = *buffer->p;
	u32 char_code = 0;

	if(u8((first_byte >> 7) | utf8_mask_1byte) == utf8_mask_1byte)
	{
		char_code = first_byte;
		buffer->p += 1;
	}
	else if (u8((first_byte >> 5) | utf8_mask_2byte) == utf8_mask_2byte and bytes_remaining >= 2)
	{
		char_code += (buffer->p[1] & 0b00111111) << 0;
		char_code += (buffer->p[0] & 0b00011111) << 6;
		buffer->p += 2;
	}
	else if (u8((first_byte >> 4) | utf8_mask_3byte) == utf8_mask_3byte and bytes_remaining >= 3)
	{
		char_code += (buffer->p[2] & 0b00111111) << 0;
		char_code += (buffer->p[1] & 0b00111111) << 6;
		char_code += (buffer->p[0] & 0b00001111) << 12;
		buffer->p += 3;
	}
	else if(u8((first_byte >> 3) | utf8_mask_4byte) == utf8_mask_4byte and bytes_remaining >= 4)
	{
		char_code += (buffer->p[3] & 0b00111111) << 0;
		char_code += (buffer->p[2] & 0b00111111) << 6;
		char_code += (buffer->p[1] & 0b00111111) << 12;
		char_code += (buffer->p[0] & 0b00000111) << 18;
		buffer->p += 4;
	}
	else
	{
		//log("Encountered invalid utf-8 starting byte, or sequence of remaining bytes was not long enough.", char_code);
		return false;
	}

	*utf32_char = char_code;
	return true;
}

u32
DigitToUtf32Char(u32 digit)
{
	if(digit < 0 or digit > 9) return 0;

	return(u32('0')+digit);
}

bool
TokenMatchesString(Token token, const char *string)
{
	if(token.length < strlen(string)) return false;

	for(int i=0; i<token.length; i++)
	{
		if(token.start[i] != string[i]) return false;

		// We're assuming that tokens are not null-terminated (the length of a token should
		// always correspond to the non-null-terminated length)
		// In this case, we've reached the end of [string] before the end of token, so we don't consider it a match.
		if(string[i] == '\0') return false;
	}

	return true;
}

// bool
// SeekAfterToken(Buffer *buffer, const char *target_string)
// {
// 	char *initial = buffer->p;

// 	size_t bytes_remaining = BufferBytesRemaining(*buffer);
// 	for(int _=0; _<bytes_remaining; _++)
// 	{

// 	}
// }

bool SkipToNextLine(Buffer *buffer)
{
	char *initial = buffer->p;

	size_t bytes_remaining = BufferBytesRemaining(*buffer);
	for(int i=0; i<bytes_remaining; i++)
	{
		if(buffer->p[0] == '\n')
		{
			++buffer->p;
			break;
		}
		++buffer->p;
	}

	if(initial != buffer->p) return true;
	else return false;
}