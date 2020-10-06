#include "better_text_parsing.h"

Token
NextToken(Buffer *buffer)
{
	Token token = {};
	char *&p = buffer->p;

	SkipChars(buffer, c::whitespace);

	if(*p == '#')
	{
		token.type = TokenType_::Pound;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '{')
	{
		token.type = TokenType_::OpenSquigglyBracket;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '}')
	{
		token.type = TokenType_::CloseSquigglyBracket;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '[')
	{
		token.type = TokenType_::OpenSquareBracket;
		token.start = p;
		token.length = 1;
	}
	else if(*p == ']')
	{
		token.type = TokenType_::CloseSquareBracket;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '<')
	{
		token.type = TokenType_::OpenAngleBracket;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '>')
	{
		token.type = TokenType_::CloseAngleBracket;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '(')
	{
		token.type = TokenType_::OpenParen;
		token.start = p;
		token.length = 1;
	}
	else if(*p == ')')
	{
		token.type = TokenType_::CloseParen;
		token.start = p;
		token.length = 1;
	}
	else if(*p == ':')
	{
		token.type = TokenType_::Colon;
		token.start = p;
		token.length = 1;
	}
	else if(*p == ';')
	{
		token.type = TokenType_::SemiColon;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '*')
	{
		token.type = TokenType_::Asterisk;
		token.start = p;
		token.length = 1;
	}
	else if(*p == ',')
	{
		token.type = TokenType_::Comma;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '~')
	{
		token.type = TokenType_::Tilde;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '=')
	{
		token.type = TokenType_::Equals;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '`')
	{
		token.type = TokenType_::Backtick;
		token.start = p;
		token.length = 1;
	}
	else if(IsAlpha(*p) or IsDigit(*p) or *p == '_')
	{ // Start of an identifier
		token.type = TokenType_::Identifier;
		token.start = p;

		// Advance until we find a non-valid identifier character.
		int i = 0;
		while(IsAlpha(p[i]) or IsDigit(p[i]) or p[i] == '_') ++i;

		token.length = i;
		//printf("%.*s\n", (int)token.length, token.start);
	}
	else
	{
		token.type = TokenType_::Unknown;
		token.start = p;
		token.length = 1;
	}

	p += token.length;
	return token;
}

bool
ConfirmNextTokenType(Buffer *buffer, TokenType_ type)
{
	char *initial = buffer->p;
	Token token = NextToken(buffer);
	if(token.type != type)
	{
		buffer->p = initial;
	}

	return(token.type == type);
}

bool
TokenMatchesString(Token token, const char *target)
{
	for(int i=0; i<token.length; i++)
	{
		if(token.start[i] != target[i]) return false;

		// We're assuming that tokens are not null-terminated (the length of a token should
		// always correspond to the non-null-terminated length)
		// In this case, we've reached the end of [target] before the end of token, so we don't consider it a match.
		if(target[i] == '\0') return false;
	}

	return true;
}

bool
TokenMatchesStringIgnoreCase(Token token, const char *target)
{
	String token_string = StringFromToken(token, &memory::per_frame_arena);
	String target_string = StringFromCString(target);

	return(CompareStrings(LowerCase(token_string), LowerCase(target_string)));
}

bool
ConfirmNextTokenAsIdentifier(Buffer *buffer, const char *target)
{
	char *initial = buffer->p;
	Token next_token = NextToken(buffer);
	if(TokenMatchesString(next_token, target))
	{
		return true;
	}
	else
	{
		buffer->p = initial;
		return false;
	}
}