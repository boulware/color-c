#ifndef TEXT_PARSING_H
#define TEXT_PARSING_H

#include "types.h"

const u8 utf8_mask_1byte = 0b0;
const u8 utf8_mask_2byte = 0b110;
const u8 utf8_mask_3byte = 0b1110;
const u8 utf8_mask_4byte = 0b11110;

struct Buffer
{
	char *data;
	char *p; // current position within *data
	size_t byte_count; // Generally, this will hide the fact that Buffer is null-terminated.
					   // byte_count should represent the number of bytes of 'real' data.
					   // As such, malloc()s should ask for byte_count+1 bytes.
};

struct StringBuffer
{
	const char *data;
	const char *p; // current position within *data
	size_t byte_count; // Generally, this will hide the fact that Buffer is null-terminated.
					   // byte_count should represent the number of bytes of 'real' data.
					   // As such, malloc()s should ask for byte_count+1 bytes.
};

StringBuffer CreateStringBuffer(const char *string);

void FreeBuffer(Buffer *buffer);

size_t BufferBytesRemaining(Buffer buffer);
size_t BufferBytesRemaining(StringBuffer buffer);

struct Token
{
	char *start;
	size_t length;
};

bool IsWhitespace(char c);
bool IsDigit(char c);
bool IsAlpha(char c);

char FirstTokenChar(Token token);
char LastTokenChar(Token token);

bool ParseNextAsS32(Buffer *buffer, s32 *value);
bool NextTokenAsDoubleQuotedString(Buffer *buffer, Token *token);

bool SeekChar(Buffer *buffer, char c);
bool SeekAfterChar(Buffer *buffer, char c);
bool SeekBeforeChar(Buffer *buffer, char c);
bool CheckNextChar(Buffer *buffer, char c);
bool ConfirmNextChar(Buffer *buffer, char c);

bool SkipChars(Buffer *buffer, const char *skip);
Token NextToken(Buffer *buffer, const char *trim);
bool ConfirmNextToken(Buffer *buffer, const char *target_string, const char *trim);
// bool SeekAfterToken(Buffer *buffer, const char *target_string);
bool SeekNextLineThatBeginsWith(Buffer *buffer, const char *start_string);
bool SeekAfterNextLineThatBeginsWith(Buffer *buffer, const char *start_string);
bool CharInString(char target, const char *string);

bool NextAsUtf32Char(StringBuffer *buffer, u32 *utf32_char);
u32 DigitToUtf32Char(u32 digit);

bool TokenMatchesString(Token token, const char *string);

#endif