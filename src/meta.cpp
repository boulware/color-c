//////////////////////////////////////////////////////////////////////////
//							TODO										//
/*

* Skipping macros that use '\' to extend past multiple lines

* Nested member (struct members that are structs themselves)
* Templated structs
* Constructor/Destructor declarations inside struct.
* function pointer members
* members that are aggregate-initialized (with {})

PARTIAL
* Templated members (templated members are implemented, but templating the MetaString() function isn't, so it's only partially complete)

FINISHED, but maybe not robust:
* Struct members that are arrays


*/////////////////////////////////////////////////////////////////////////

#include "meta.h"

#include "math.h"

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"
#include "meta_text_parsing.h"
#include "meta_text_parsing.cpp"
#include "math.cpp"

#define Fmt_first(type, fmt)\
if(base_meta_type == MetaType::_##type) {base_type_format_specifier = #fmt[0];}\
else if(base_meta_type == MetaType::_##type##_pointer) {base_type_format_specifier = 'p';}

#define Fmt(type, fmt)\
else if(base_meta_type == MetaType::_##type) {base_type_format_specifier = #fmt[0];}\
else if(base_meta_type == MetaType::_##type##_pointer) {base_type_format_specifier = 'p';}

Buffer
LoadFileIntoMemoryAndNullTerminate(const char *filename)
{
	Buffer buffer;

	FILE *file = fopen(filename, "r");

	if(!file)
	{
		printf("Failed to open file: %s\n", filename);
		return Buffer{};
	}

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
FindHeaderFilesAndOutputIncludes(const char *dir, FILE *out)
{
	WIN32_FIND_DATA file_data;
	HANDLE handle = FindFirstFileA(dir, &file_data);

	while(GetLastError() != ERROR_NO_MORE_FILES)
	{
		// Skip win32_ files to avoid #include <windows.h> megapollution
		if(    strncmp("win32_", file_data.cFileName, 6) != 0
		   and strncmp("meta_", file_data.cFileName, 5) != 0)
		{
			fprintf(out, "#include \"%s\"\n", file_data.cFileName);
		}

		FindNextFileA(handle, &file_data);
	}

	SetLastError(0);
	FindClose(handle);
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
		else if(p[0] == '/' and p[1] == '*')
		{
			while(p != '\0' or !(p[0] == '*' and p[1] == '/'))
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
	else if(IsAlpha(*p) or *p == '_')
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
SeekMatchingBracket(Buffer *buffer)
{
	char *initial = buffer->p;
	char *&p = buffer->p; // alias

	Token open_bracket_token = NextToken(buffer);
	bool success = false;
	char *end_bracket_pos = nullptr;
	if(open_bracket_token.type == TokenType_::OpenSquigglyBracket)
	{
		// '{'
		int nested_bracket_count = 0;
		while(BufferBytesRemaining(*buffer) > 0)
		{
			Token token = NextToken(buffer);
			if(token.type == TokenType_::OpenSquigglyBracket)
			{
				++nested_bracket_count;
			}
			else if(token.type == TokenType_::CloseSquigglyBracket)
			{
				if(nested_bracket_count == 0)
				{
					success = true;
					end_bracket_pos = token.start;
					break;
				}
				else
				{
					--nested_bracket_count;
				}
			}
		}
	}
	else if(open_bracket_token.type == TokenType_::OpenSquareBracket)
	{
		// '{'
		int nested_bracket_count = 0;
		while(BufferBytesRemaining(*buffer) > 0)
		{
			Token token = NextToken(buffer);
			if(token.type == TokenType_::OpenSquareBracket)
			{
				++nested_bracket_count;
			}
			else if(token.type == TokenType_::CloseSquareBracket)
			{
				if(nested_bracket_count == 0)
				{
					success = true;
					end_bracket_pos = token.start;
					break;
				}
				else
				{
					--nested_bracket_count;
				}
			}
		}
	}
	else if(open_bracket_token.type == TokenType_::OpenAngleBracket)
	{
		// '{'
		int nested_bracket_count = 0;
		while(BufferBytesRemaining(*buffer) > 0)
		{
			Token token = NextToken(buffer);
			if(token.type == TokenType_::OpenAngleBracket)
			{
				++nested_bracket_count;
			}
			else if(token.type == TokenType_::CloseAngleBracket)
			{
				if(nested_bracket_count == 0)
				{
					success = true;
					end_bracket_pos = token.start;
					break;
				}
				else
				{
					--nested_bracket_count;
				}
			}
		}
	}

	// Set final buffer position to end of bracket
	if(end_bracket_pos) buffer->p = end_bracket_pos;

	if(!success)
	{
		// Reset buffer position if no matching bracket found.
		buffer->p = initial;
	}
	return success;
}

bool
ConfirmIdentifier(Buffer *buffer, const char *string)
{
	Token token = NextToken(buffer);
	return(token.type == TokenType_::Identifier and TokenMatchesString(token, string));
}

bool
ConfirmTokenType_(Buffer *buffer, TokenType_ type)
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
CheckNextTokenType_(Buffer *buffer, TokenType_ type)
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
	_bool,
	_bool_pointer,
	_int,
	_int_pointer,
	_s8,
	_s8_pointer,
	_s16,
	_s16_pointer,
	_s32,
	_s32_pointer,
	_s64,
	_s64_pointer,
	_u8,
	_u8_pointer,
	_u16,
	_u16_pointer,
	_u32,
	_u32_pointer,
	_u64,
	_u64_pointer,
	_char,
	_char_pointer,
	_void,
	_void_pointer,
	_float,
	_float_pointer,
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

#define mInterpretToken(type) if(TokenMatchesString(token, #type)) {if(!is_pointer) {meta_type=MetaType::_##type;} else {meta_type=MetaType::_##type##_pointer;}}

MetaType
InterpretTokenAsMetaType(Token token, bool is_pointer)
{
	MetaType meta_type = MetaType::unknown;

	mInterpretToken(int)
	mInterpretToken(char)
	mInterpretToken(bool)
	mInterpretToken(void)
	mInterpretToken(float)
	mInterpretToken(s8)
	mInterpretToken(s16)
	mInterpretToken(s32)
	mInterpretToken(s64)
	mInterpretToken(u8)
	mInterpretToken(u16)
	mInterpretToken(u32)
	mInterpretToken(u64)

	return meta_type;
}


int main()
{
	#define root_dir "D:/work/programming/color-c/"

	FILE *out_file = fopen(root_dir "src/meta_print.cpp", "w");
	if(!out_file)
	{
		printf("Failed to open meta_print.cpp file");
		return 0;
	}
	fprintf(out_file, "#include \"meta_print.h\"\n\n");
	FindHeaderFilesAndOutputIncludes(root_dir "src/*.h", out_file);

	FILE *header_file = fopen(root_dir "src/meta_print.h", "w");
	if(!header_file)
	{
		printf("Failed to open meta_print.h file");
		return 0;
	}
	FindHeaderFilesAndOutputIncludes(root_dir "src/*.h", header_file);
	fprintf(header_file, "\n");

	WIN32_FIND_DATA file_data;
	HANDLE handle = FindFirstFileA(root_dir "src/*.h", &file_data);

	while(GetLastError() != ERROR_NO_MORE_FILES)
	{
		// Skip win32_ files to avoid #include <windows.h> megapollution
		if(strncmp("win32_", file_data.cFileName, 6) == 0)
		{
			FindNextFileA(handle, &file_data);
			continue;
		}

		char filename_with_folder[1024] = {};
		{ // Build filename_with_folder, by prepending "src/" to cFileName
			size_t path_length = strlen(root_dir "src/");
			strncpy(filename_with_folder, root_dir "src/", path_length);
			strcpy(filename_with_folder+path_length, file_data.cFileName);
		}

		Buffer file = LoadFileIntoMemoryAndNullTerminate(filename_with_folder);

		fprintf(out_file, "\n// ---------------FILE START---------------\n");
		fprintf(out_file, "// %s\n", file_data.cFileName);
		fprintf(out_file, "// ------------------------------------------\n");
		// fprintf(out_file, "#include <string.h>\n");
		// fprintf(out_file, "#include \"utf32string.h\"\n");
		//fprintf(out_file, "\n");

		bool is_parsing = true;
		while(is_parsing and BufferBytesRemaining(file) > 0)
		{
			char *&p = file.p;
			Token token = NextToken(&file);

			if(token.type == TokenType_::Unknown)
			{
				// #if META_PRINT
				// printf("Invalid token encountered (token %.*s, file %s) [\"%.100s\"]\n",
				// 	(int)token.length, token.start,
				// 	file_data.cFileName,
				// 	file.p);
				// #endif
				continue;
			}
			else if(token.type == TokenType_::Pound)
			{ // Skip to next line on encountering macro (doesn't support '\'' line continuations)
				while(*p != '\n' and BufferBytesRemaining(file) > 0) ++p;
			}
			else if(token.type == TokenType_::Identifier)
			{
				bool struct_is_templated = false;
				if(TokenMatchesString(token, "template"))
				{
					bool valid = true;
					valid = valid && ConfirmTokenType_(&file, TokenType_::OpenAngleBracket);
					SeekChar(&file, '<');
					SeekMatchingBracket(&file);
					valid = valid && ConfirmTokenType_(&file, TokenType_::CloseAngleBracket);

					Token thing_being_templated = NextToken(&file);
					if(TokenMatchesString(thing_being_templated, "struct"))
					{
						// This template corresponds to a templated struct, so we want
						// to template the MetaString function to match it.
						file.p = thing_being_templated.start;
						token = NextToken(&file);
						struct_is_templated = true;
					}
				}

				if(TokenMatchesString(token, "struct"))
				{ // Try to parse from here as a struct.
					fprintf(out_file, "\n");
					StructMetaData meta_data = {};

					bool valid = true;

					// struct identifier
					//valid = valid && ConfirmIdentifier(&file, "struct");

					// struct name
					Token struct_name_token = NextToken(&file);
					if(struct_name_token.type != TokenType_::Identifier)
					{
						// I don't think this should ever happen in valid C syntax, so if it does, let's warn about it.
						printf("Encountered non-identifier after struct keyword: %.*s",
							   (int)struct_name_token.length, struct_name_token.start);
						break;
					}
					if(CheckNextTokenType_(&file, TokenType_::SemiColon)) continue; // Skip struct declarations
					// meta_data.name = (char*)malloc(struct_name_token.length+1);
					// memcpy(meta_data.name, struct_name_token.start, struct_name_token.length);
					// meta_data.name[struct_name_token.length] = '\0';

					if(struct_is_templated)
					{
						fprintf(header_file, "template<typename Type>\n");
						fprintf(out_file, "template<typename Type>\n");
					}

					// Output declaration to H file
					fprintf(header_file, "String MetaString(const %.*s *s);\n\n", int(struct_name_token.length), struct_name_token.start);

					// Output beginning of definition to CPP file.
					fprintf(out_file, "String MetaString(const %.*s *s)\n", int(struct_name_token.length), struct_name_token.start);
					fprintf(out_file, "{\n");

					fprintf(out_file, "\tTIMED_BLOCK;\n");
					fprintf(out_file, "\n");

					fprintf(out_file, "\tString string = {};\n");
					fprintf(out_file, "\tstring.length = 0;\n");
					fprintf(out_file, "\tstring.max_length = 1024;\n");
					fprintf(out_file, "\tstring.data = ScratchString(string.max_length);\n");
					fprintf(out_file, "\n");
					fprintf(out_file, "\tAppendCString(&string, \"%.*s {\\n\");\n\n",
							(int)struct_name_token.length, struct_name_token.start);

					// PrintToken(struct_name_token);
					// printf("\n");

					// struct opening bracket
					valid = valid && ConfirmTokenType_(&file, TokenType_::OpenSquigglyBracket);

					// struct members
					while(BufferBytesRemaining(file) > 0)
					{
						// if(meta_data.member_count >= ArrayCount(meta_data.members))
						// {
						// 	printf("Encountered more than allowed max members in struct: %s", meta_data.name);
						// 	break;
						// }

						Token base_type_token = NextToken(&file);

						// @note: this will break for explicit nested struct members (anonymous struct)
						if(base_type_token.type == TokenType_::CloseSquigglyBracket) break;

						// Loop over comma-separated members here.
						bool in_comma_separated_list = true;
						while(in_comma_separated_list)
						{
							bool is_pointer;
							char no_pointer[] = "";
							char yes_pointer[] = " *";
							char *pointer_character_string = nullptr;
							if(CheckNextTokenType_(&file, TokenType_::Asterisk))
							{ // Pointer
								ConfirmTokenType_(&file, TokenType_::Asterisk);
								pointer_character_string = yes_pointer;
								is_pointer = true;
							}
							else
							{
								pointer_character_string = no_pointer;
								is_pointer = false;
							}

							// Check if the member is templated.
							if(CheckNextTokenType_(&file, TokenType_::OpenAngleBracket))
							{
								SeekMatchingBracket(&file);
								if(ConfirmTokenType_(&file, TokenType_::CloseAngleBracket))
								{
									// Extend base_type_token to include the templated part.
									base_type_token.length = file.p - base_type_token.start;
								}
								else
								{
									printf("SeekMatchingBracket() failed. (meta.cpp ln: %d)", __LINE__);
								}
							}

							if(CheckNextTokenType_(&file, TokenType_::Identifier))
							{ // Member name (no current support for multi-identifier declarations, like const, volatile, etc.)
								Token member_name = NextToken(&file);

								char yes_array[] = "[]";
								if(CheckNextTokenType_(&file, TokenType_::OpenSquareBracket))
								{ // Array member
									pointer_character_string = yes_array;
								}



								MetaType base_meta_type = MetaType::unknown;
								base_meta_type = InterpretTokenAsMetaType(base_type_token, is_pointer);

								char base_type_format_specifier = 'd';

								bool known_meta_type = true;

								Fmt_first(int, d)
								Fmt(char, c)
								Fmt(bool, d)
								Fmt(void, d)
								Fmt(float, f)
								Fmt(s8, d)
								Fmt(s16, d)
								Fmt(s32, d)
								Fmt(s64, d)
								Fmt(u8, u)
								Fmt(u16, u)
								Fmt(u32, u)
								Fmt(u64, u)
								else
								{
									// We found an unknown base meta type, so printf the error...
									// printf("Undefined base meta type (%.*s) found in struct \"%.*s\" (file: %s)\n",
									// 	   (int)base_type_token.length, base_type_token.start,
									// 	   (int)struct_name_token.length, struct_name_token.start,
									// 	   file_data.cFileName);

									known_meta_type = false;
								}

								if(known_meta_type)
								{
									fprintf(out_file, "\tAppendCString(&string, \"  %.*s: %%%c (%.*s%s)\\n\", s->%.*s);\n\n",
											(int)member_name.length, member_name.start,
											base_type_format_specifier,
											(int)base_type_token.length, base_type_token.start,
											pointer_character_string,
											(int)member_name.length, member_name.start);
								}
								else
								{
									// Append the member as a string from a recursive MetaString() call.
									fprintf(out_file, "\tAppendCString(&string, \"  %.*s: \");\n",
										    (int)member_name.length, member_name.start);
									fprintf(out_file, "\tAppendString(&string, MetaString(&s->%.*s));\n",
											(int)member_name.length, member_name.start);
									fprintf(out_file, "\tAppendCString(&string, \"(%.*s)\\n\");\n\n",
											(int)base_type_token.length, base_type_token.start);
								}
							}

							if(CheckNextTokenType_(&file, TokenType_::Comma))
							{
								ConfirmTokenType_(&file, TokenType_::Comma);
							}
							else
							{
								in_comma_separated_list = false;
							}
						}

						if(CheckNextTokenType_(&file, TokenType_::OpenSquareBracket))
						{ // Array member
							SeekMatchingBracket(&file);
							if(!CheckNextTokenType_(&file, TokenType_::CloseSquareBracket))
							{
								printf("SeekMatchingBracket() failed to find matching square bracket.");
							}
							else ConfirmTokenType_(&file, TokenType_::CloseSquareBracket);
						}

						if(!ConfirmTokenType_(&file, TokenType_::SemiColon))
						{
							#if META_PRINT
							printf("Invalid token encountered after struct member definition. (type %.*s, struct %.*s, file %s):\n",
								(int)base_type_token.length, base_type_token.start,
								(int)struct_name_token.length, struct_name_token.start,
								file_data.cFileName);
							printf("---------------------\n");
							printf("%.*s\n", m::Min((int)100, (int)BufferBytesRemaining(file)), file.p);
							printf("---------------------\n\n\n");
							#endif

							SeekAfterChar(&file, ';');
							continue;
						}
					}

					fprintf(out_file, "\tAppendCString(&string, \"}\");\n");
					fprintf(out_file, "\n");
					fprintf(out_file, "\treturn string;\n");
					fprintf(out_file, "}\n");
				}
			}
		}

		FreeBuffer(&file);
		FindNextFileA(handle, &file_data);
	}

	FindClose(handle);
	fclose(out_file);
	fclose(header_file);
		// if(token.type == TokenType_::Identifier)
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