#ifndef BETTER_TEXT_PARSING_H
#define BETTER_TEXT_PARSING_H

// struct Buffer
// {
// 	char *data;
// 	char *p;
// 	size_t byte_count;
// };

// enum class TokenType_
// {
// 	Unknown,
// 	Pound,
// 	OpenSquigglyBracket,
// 	CloseSquigglyBracket,
// 	OpenSquareBracket,
// 	CloseSquareBracket,
// 	OpenAngleBracket,
// 	CloseAngleBracket,
// 	OpenParen,
// 	CloseParen,
// 	Colon,
// 	SemiColon,
// 	Asterisk,
// 	Comma,
// 	Tilde,
// 	Equals,
// 	Identifier
// };

// struct Token
// {
// 	TokenType_ type;
// 	char *start;
// 	size_t length;
// };

//Buffer BufferFromCString(const char *string);

Token NextToken(Buffer *buffer);
bool ConfirmNextTokenType(Buffer *buffer, TokenType_ type);
bool TokenMatchesString(Token token, const char *string);
bool TokenMatchesStringIgnoreCase(Token token, const char *target);
bool ConfirmNextTokenAsIdentifier(Buffer *buffer, const char *string);

#endif