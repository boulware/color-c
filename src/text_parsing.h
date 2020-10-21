#ifndef TEXT_PARSING_H
#define TEXT_PARSING_H

#include "types.h"
#include "memory.h"
#include "string.h"
#include "buffer.h"

const u8 utf8_mask_1byte = 0b0;
const u8 utf8_mask_2byte = 0b110;
const u8 utf8_mask_3byte = 0b1110;
const u8 utf8_mask_4byte = 0b11110;



enum class TokenType_
{
	Unknown,
	Pound,
	OpenSquigglyBracket,
	CloseSquigglyBracket,
	OpenSquareBracket,
	CloseSquareBracket,
	OpenAngleBracket,
	CloseAngleBracket,
	OpenParen,
	CloseParen,
	Colon,
	SemiColon,
	Asterisk,
	Comma,
	Tilde,
	Equals,
	Backtick,
	Identifier
};

struct Token
{
	TokenType_ type;
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
Token NextToken_(Buffer *buffer, const char *trim);
bool ConfirmNextToken(Buffer *buffer, const char *target_string, const char *trim);
// bool SeekAfterToken(Buffer *buffer, const char *target_string);
bool SeekNextLineThatBeginsWith(Buffer *buffer, const char *start_string);
bool SeekAfterNextLineThatBeginsWith(Buffer *buffer, const char *start_string);
bool CharInString(char target, const char *string);

bool NextAsUtf32Char(Buffer *buffer, u32 *utf32_char);
u32 DigitToUtf32Char(u32 digit);

//bool TokenMatchesString(Token token, const char *string);

String StringFromToken(Token token, Id<Arena> arena_id = memory::per_frame_arena_id);

bool TokenMatchesString(Token token, const char *string);

#endif