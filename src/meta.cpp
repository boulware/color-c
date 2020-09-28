#include "meta.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "meta_text_parsing.h"
#include "meta_text_parsing.cpp"

Buffer
LoadFileIntoMemoryAndNullTerminate(const char *filename)
{
	Buffer buffer;

	FILE *file = fopen(filename, "r");

	if(!file) return Buffer{};

	fseek(file, 0, SEEK_END);
	size_t file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *data = (char *)malloc(file_size+1);
	fread(data, 1, file_size, file);
	fclose(file);

	data[file_size] = 0;

	buffer.data = data;
	buffer.p = data;
	buffer.byte_count = file_size;

	return buffer;
}

void
ParseAsStruct(char *buffer)
{
	char *p = buffer;


}

void
SkipWhitespaceAndComments(Buffer *buffer)
{
	char *&p = buffer->p;

	while(BufferBytesRemaining(*buffer) > 0)
	{
		if(IsWhitespace(*p))
		{
			++p;
			continue;
		}
		else if(p[0] == '/' and p[1] == '/')
		{
			while(*p != '\n' and *p != '\0')
			{
				++p;
			}
			continue;
		}
		else return;
	}
}

Token
NextToken(Buffer *buffer)
{
	Token token = {};
	char *&p = buffer->p;

	SkipWhitespaceAndComments(buffer);

	if(*p == '#')
	{
		token.type = TokenType::Pound;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '{')
	{
		token.type = TokenType::OpenBracket;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '}')
	{
		token.type = TokenType::CloseBracket;
		token.start = p;
		token.length = 1;
	}
	else if(*p == ';')
	{
		token.type = TokenType::SemiColon;
		token.start = p;
		token.length = 1;
	}
	else if(*p == '*')
	{
		token.type = TokenType::Asterisk;
		token.start = p;
		token.length = 1;
	}
	else if(IsAlpha(*p) or *p == '_')
	{ // Start of an identifier
		token.type = TokenType::Identifier;
		token.start = p;

		// Advance until we find a non-valid identifier character.
		int i = 0;
		while(IsAlpha(p[i]) or IsDigit(p[i]) or p[i] == '_') ++i;

		token.length = i;
		//printf("%.*s\n", (int)token.length, token.start);
	}
	else
	{
		token.type = TokenType::Unknown;
		token.start = p;
		token.length = 1;
	}

	p += token.length;
	return token;
}

bool
ConfirmIdentifier(Buffer *buffer, const char *string)
{
	Token token = NextToken(buffer);
	return(token.type == TokenType::Identifier and TokenMatchesString(token, string));
}

bool
ConfirmTokenType(Buffer *buffer, TokenType type)
{
	Token token = NextToken(buffer);
	return(token.type == type);
}

bool
CheckNextTokenType(Buffer *buffer, TokenType type)
{
	char *initial = buffer->p;
	Token token = NextToken(buffer);
	buffer->p = initial;
	return(token.type == type);
}

void
PrintToken(Token token)
{
	printf("%.*s", (int)token.length, token.start);
}

enum class MetaType
{
	unknown,
	_int,
	_u32,
	_u32_pointer,
};

struct StructMember
{
	char *name;
	MetaType type;
};

struct StructMetaData
{
	char *name;
	size_t member_count;
	StructMember members[256];
};

// MetaType
// InterpretTokenAsMetaType(Token token)
// {
// 	MetaType meta_type = MetaType::unknown;

// 	if(!is_pointer)
// 	{
// 		if(TokenMatchesString(base_type_token, "int"))
// 		{
// 			base_meta_type = MetaType::_int;
// 		}
// 		if(TokenMatchesString(base_type_token, "u32"))
// 		{
// 			base_meta_type = MetaType::_u32;
// 		}
// 	}
// 	else
// 	{
// 		if(TokenMatchesString(base_type_token, "int"))
// 		{
// 			base_meta_type = MetaType::_int_pointer;
// 		}
// 		if(TokenMatchesString(base_type_token, "u32"))
// 		{
// 			base_meta_type = MetaType::_u32_pointer;
// 		}
// 	}
// }


int main()
{
	Buffer file = LoadFileIntoMemoryAndNullTerminate("src/utf32string.h");

	FILE *out_file = fopen("src/meta_print.cpp", "w");
	if(!out_file)
	{
		printf("Failed to open meta_print.cpp file");
		return 0;
	}

	fprintf(out_file, "#include <string.h>\n");
	fprintf(out_file, "#include \"utf32string.h\"\n");
	fprintf(out_file, "\n");

	bool is_parsing = true;
	while(is_parsing and BufferBytesRemaining(file) > 0)
	{
		char *&p = file.p;
		Token token = NextToken(&file);

		if(token.type == TokenType::Unknown)
		{
			break;
		}
		else if(token.type == TokenType::Pound)
		{ // Skip to next line on encountering macro (doesn't support '\'' line continuations)
			while(*p != '\n') ++p;
		}
		else if(token.type == TokenType::Identifier)
		{
			if(TokenMatchesString(token, "Introspect"))
			{ // Try to parse from here as a struct.
				StructMetaData meta_data = {};

				bool valid = true;

				// struct identifier
				valid = valid && ConfirmIdentifier(&file, "struct");

				// struct name
				Token struct_name_token = NextToken(&file);
				if(struct_name_token.type != TokenType::Identifier) break;
				meta_data.name = (char*)malloc(struct_name_token.length+1);
				memcpy(meta_data.name, struct_name_token.start, struct_name_token.length);
				meta_data.name[struct_name_token.length] = '\0';

				fprintf(out_file, "String AsString(const %.*s *s)\n", int(struct_name_token.length), struct_name_token.start);
				fprintf(out_file, "{\n");

				fprintf(out_file, "\tString string = {};\n");
				fprintf(out_file, "\tstring.length = 0;\n");
				fprintf(out_file, "\tstring.max_length = 1024;\n");
				fprintf(out_file, "\tstring.data = ScratchString(string.max_length);\n");
				fprintf(out_file, "\n");
				fprintf(out_file, "\tAppendCString(&string, \"%.*s {\\n\");\n",
						(int)struct_name_token.length, struct_name_token.start);

				PrintToken(struct_name_token);
				printf("\n");

				// struct opening bracket
				valid = valid && ConfirmTokenType(&file, TokenType::OpenBracket);

				// struct members
				while(BufferBytesRemaining(file) > 0)
				{
					if(meta_data.member_count >= ArrayCount(meta_data.members))
					{
						printf("Encountered more than allowed max members in struct: %s", meta_data.name);
						break;
					}

					Token base_type_token = NextToken(&file);
					if(base_type_token.type == TokenType::CloseBracket) break;

					bool is_pointer = false;
					if(CheckNextTokenType(&file, TokenType::Asterisk))
					{ // Pointer
						ConfirmTokenType(&file, TokenType::Asterisk);
						is_pointer = true;
					}

					if(CheckNextTokenType(&file, TokenType::Identifier))
					{ // Member name (no current support for multi-identifier declarations, like const, volatile, etc.)
						Token member_name = NextToken(&file);

						StructMember *member = &meta_data.members[meta_data.member_count];
						member->name = (char*)malloc(member_name.length+1);
						memcpy(member->name, member_name.start, member_name.length);
						member->name[member_name.length] = '\0';

						MetaType base_meta_type = MetaType::unknown;
						//MetaType base_meta_type = InterpretTokenAsMetaType(base_type_token);

						fprintf(out_file, "\tAppendCString(&string, \"  %.*s: %%d (%.*s)\\n\", s.%.*s);\n",
								(int)member_name.length, member_name.start,
								(int)base_type_token.length, base_type_token.start,
								(int)member_name.length, member_name.start);

					}

					if(!ConfirmTokenType(&file, TokenType::SemiColon))
					{
						printf("Invalid token encountered after struct member definition.");
						break;
					}
				}

			}
		}
	}

	fclose(out_file);
		// if(token.type == TokenType::Identifier)
		// {

		// }

	// bool valid = true;
	// valid = valid && SeekNextLineThatBeginsWith(&file, "Introspect");
	// char *initial = file.p;
	// valid = valid && ConfirmNextToken(&file, "Introspect");
	// valid = valid && ConfirmNextToken(&file, "struct");
	// if(!valid)
	// {
	// 	printf("Invalid struct format: %.*s", int(file.p-initial), initial);
	// 	return 0;
	// }

	// Token struct_name_token = NextToken(&file);
	// printf("struct name: %.*s\n", (int)struct_name_token.length, struct_name_token.start);

	// valid = valid && ConfirmNextToken(&file, "{");
	// if(!valid)
	// {
	// 	printf("Invalid struct format: %.*s", int(file.p-initial), initial);
	// 	return 0;
	// }

	// bool is_parsing = true;
	// while(is_parsing and BufferBytesRemaining(file) > 0)
	// {
	// 	Token type_token = NextToken(&file);
	// 	Token member_name_token = NextToken(&file, " \t\n\v\f\r;");
	// 	if(!ConfirmNextChar(&file, ';'))
	// 	{
	// 		printf("Invalid struct format: %.*s", int(file.p-initial), initial);
	// 		return 0;
	// 	}

	// 	printf("member: %.*s (type %.*s)\n",
	// 		   (int)member_name_token.length, member_name_token.start,
	// 		   (int)type_token.length, type_token.start);

	// }
}