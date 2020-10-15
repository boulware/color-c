//////////////////////////////////////////////////////////////////////////
//							TODO										//
/*

* Skipping macros that use '\' to extend past multiple lines
* Skipping #if 0 statements

* Nested member (struct members that are structs themselves)
* Templated structs
* Constructor/Destructor declarations inside struct.
* function pointer members
* members that are aggregate-initialized (with {})
* show all members of an array instead of just the address of it
* parse enum classes so that their MetaString prints the actual enum name (not the integer).

PARTIAL
* Templated members (templated members are implemented, but templating the MetaString() function isn't, so it's only partially complete)

FINISHED, but maybe not robust:
* Struct members that are arrays
* implement NextTokenMatchesString(), and similarly for token sequences:
           e.g., NextTokensMatchStrings(&file, "enum", "class");


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
if(base_meta_type == MetaType::_##type) {base_type_format_specifier = #fmt;}\
else if(base_meta_type == MetaType::_##type##_pointer) {base_type_format_specifier = "p";}

#define Fmt(type, fmt)\
else if(base_meta_type == MetaType::_##type) {base_type_format_specifier = #fmt;}\
else if(base_meta_type == MetaType::_##type##_pointer) {base_type_format_specifier = "p";}

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

	char *data = (char *)calloc(file_size+1, 1);
	fread(data, 1, file_size, file);
	fclose(file);

	data[file_size] = 0;

	buffer.data = data;
	buffer.p = data;
	buffer.byte_count = file_size;

	return buffer;
}

void
FindHeaderFilesAndOutputIncludes(const char *dir, Buffer *out)
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
	else if(*p == ':')
	{
		token.type = TokenType_::Colon;
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
NextTokenMatchesString(Buffer *buffer, const char *string)
{
	char *initial = buffer->p;
	Token token = NextToken(buffer);
	buffer->p = initial;

	return(TokenMatchesString(token, string));
}

bool
NextTokensMatchStrings(Buffer *buffer, const char *s0, const char *s1)
{
	char *initial = buffer->p;
	Token t0 = NextToken(buffer);
	Token t1 = NextToken(buffer);
	buffer->p = initial;

	return(TokenMatchesString(t0, s0) and TokenMatchesString(t1, s1));
}

void
SkipTokens(Buffer *buffer, int count)
{
	for(int i=0; i<count; i++)
	{
		NextToken(buffer);
	}
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
		// '['
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
		// '<'
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
	else if(open_bracket_token.type == TokenType_::OpenParen)
	{
		// '('
		int nested_bracket_count = 0;
		while(BufferBytesRemaining(*buffer) > 0)
		{
			Token token = NextToken(buffer);
			if(token.type == TokenType_::OpenParen)
			{
				++nested_bracket_count;
			}
			else if(token.type == TokenType_::CloseParen)
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

	// Set final buffer position to end at the bracket (not past it)
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
	_size_t,
	_size_t_pointer,
	_GLuint,
	_GLuint_pointer,
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
	mInterpretToken(GLuint)
	mInterpretToken(size_t)

	return meta_type;
}


int main()
{
	#define root_dir "C:/color-c/"

	const size_t buffer_size = 10000000; // 10MB

	Buffer out_file = {};
	out_file.data = (char *)calloc(buffer_size, 1);
	out_file.p = out_file.data;
	out_file.byte_count = buffer_size;

	Buffer header_file = {};
	header_file.data = (char *)calloc(buffer_size, 1);
	header_file.p = header_file.data;
	header_file.byte_count = buffer_size;

	fprintf(&out_file, "#include \"meta_print.h\"\n\n");
	FindHeaderFilesAndOutputIncludes(root_dir "src/*.h", &out_file);
	FindHeaderFilesAndOutputIncludes(root_dir "src/*.h", &header_file);

	WIN32_FIND_DATA file_data;
	HANDLE handle = FindFirstFileA(root_dir "src/*.h", &file_data);


	while(GetLastError() != ERROR_NO_MORE_FILES)
	{
		// Skip win32_ files to avoid #include <windows.h> megapollution
		if(   strncmp("win32_", file_data.cFileName, 6) == 0
           or strncmp("meta_", file_data.cFileName, 5) == 0
           or strncmp("opengl.h", file_data.cFileName, 8) == 0
           or strncmp("platform.h", file_data.cFileName, 10) == 0)
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

		// const char *stop_string = "game_state.h";
		// if(strncmp(stop_string, file_data.cFileName, strlen(stop_string)) == 0) return 0;

		Buffer file = LoadFileIntoMemoryAndNullTerminate(filename_with_folder);

		fprintf(&out_file, "\n// ---------------FILE START---------------\n");
		fprintf(&out_file, "// %s\n", file_data.cFileName);
		fprintf(&out_file, "// ------------------------------------------\n");
		// fprintf(&out_file, "#include <string.h>\n");
		// fprintf(&out_file, "#include \"utf32string.h\"\n");
		//fprintf(&out_file, "\n");

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
				if(TokenMatchesString(token, "enum") and NextTokenMatchesString(&file, "class"))
				{ // enum class
					SkipTokens(&file, 1);
					Token enum_class_name_token = NextToken(&file);

					if(CheckNextTokenType_(&file, TokenType_::Colon))
					{ // enum type specification (e.g., enum class Color : u8 {})
						SkipTokens(&file, 2); // skip colon and type name
					}

					bool valid = true;
					valid = valid && ConfirmTokenType_(&file, TokenType_::OpenSquigglyBracket);
					if(!valid) continue;

					// Output declaration to H file
					fprintf(&header_file, "String MetaString(const %.*s *s);\n\n",
						    int(enum_class_name_token.length), enum_class_name_token.start);

					// Output beginning of definition to CPP file.
					fprintf(&out_file, "\nString MetaString(const %.*s *s)\n",
						    int(enum_class_name_token.length), enum_class_name_token.start);
					fprintf(&out_file, "{\n");

					fprintf(&out_file, "\tTIMED_BLOCK;\n");
					fprintf(&out_file, "\n");

					fprintf(&out_file, "\tString string = {};\n");
					fprintf(&out_file, "\tstring.length = 0;\n");
					fprintf(&out_file, "\tstring.max_length = 1024;\n");
					fprintf(&out_file, "\tstring.data = ScratchString(string.max_length);\n");
					fprintf(&out_file, "\n");
					fprintf(&out_file, "\tAppendCString(&string, \"%.*s::\");\n",
							(int)enum_class_name_token.length, enum_class_name_token.start);

					fprintf(&out_file, "\tswitch(*s)\n");
					fprintf(&out_file, "\t{\n");

					bool parsing = true;
					while(parsing)
					{
						Token member_name_token = NextToken(&file);
						if(member_name_token.type == TokenType_::CloseSquigglyBracket)
						{
							break;
						}

						if(CheckNextTokenType_(&file, TokenType_::Comma))
						{
					        SeekAfterChar(&file, ',');
						}
						else
						{
							parsing = false;
						}


						fprintf(&out_file, "\t\tcase(%.*s::%.*s): {\n",
							    (int)enum_class_name_token.length, enum_class_name_token.start,
							    (int)member_name_token.length, member_name_token.start);
						fprintf(&out_file, "\t\t\tAppendCString(&string, \"%.*s\");\n",
							    (int)member_name_token.length, member_name_token.start);
						fprintf(&out_file, "\t\t} break;\n");
					}

					fprintf(&out_file, "\t\tdefault: {\n");
					fprintf(&out_file, "\t\t\tAppendCString(&string, \"?????\");\n");
					fprintf(&out_file, "\t\t} break;\n");
					fprintf(&out_file, "\t}\n");
					fprintf(&out_file, "\n");
					fprintf(&out_file, "\treturn string;\n");
					fprintf(&out_file, "}\n");

				}

				bool struct_is_templated = false;
				if(TokenMatchesString(token, "template"))
				{
					bool valid = true;
					//Token open_angle = NextToken(&file);
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

				if(TokenMatchesString(token, "NoIntrospect"))
				{
					token = NextToken(&file); // expected "struct"
					token = NextToken(&file); // skipping struct
				}

				if(TokenMatchesString(token, "struct"))
				{ // Try to parse from here as a struct.
					fprintf(&out_file, "\n");
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

					char *template_type_string = "";
					if(struct_is_templated)
					{
						fprintf(&header_file, "template<typename Type>\n");
						fprintf(&out_file, "template<typename Type>\n");
						template_type_string = "<Type>";
					}

					// Output declaration to H file
					fprintf(&header_file, "String MetaString(const %.*s%s *s);\n\n",
						    int(struct_name_token.length), struct_name_token.start,
						    template_type_string);

					// Output beginning of definition to CPP file.
					fprintf(&out_file, "String MetaString(const %.*s%s *s)\n",
						    int(struct_name_token.length), struct_name_token.start,
						    template_type_string);
					fprintf(&out_file, "{\n");

					fprintf(&out_file, "\tTIMED_BLOCK;\n");
					fprintf(&out_file, "\n");

					fprintf(&out_file, "\tString string = {};\n");
					fprintf(&out_file, "\tstring.length = 0;\n");
					fprintf(&out_file, "\tstring.max_length = 1024;\n");
					fprintf(&out_file, "\tstring.data = ScratchString(string.max_length);\n");
					fprintf(&out_file, "\n");
					fprintf(&out_file, "\tAppendCString(&string, \"%.*s {\\n\");\n\n",
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

						bool member_is_const = false;
						if(TokenMatchesString(base_type_token, "const"))
						{
							base_type_token = NextToken(&file);
							member_is_const = true;
						}
						else if(TokenMatchesString(base_type_token, "volatile"))
						{
							base_type_token = NextToken(&file);
						}

						if(CheckNextTokenType_(&file, TokenType_::Tilde))
						{ // Destructor declaration
							// Skip over destructor name, and the function signature check below will
							// catch the rest of this and parse the rest as a function declaration or definition.
							Token destructor_name = NextToken(&file);
						}

						// Check if it's a function signature.
						if(CheckNextTokenType_(&file, TokenType_::OpenParen))
						{
							SeekMatchingBracket(&file);
							if(ConfirmTokenType_(&file, TokenType_::CloseParen))
							{
								Token token = NextToken(&file);

								if(token.type == TokenType_::SemiColon)
								{ // Function declaration. Skip to next member
									continue;
								}
								else if(token.type == TokenType_::OpenSquigglyBracket)
								{ // Function definition. Skip to matching end brace, then go to next member.
									file.p = token.start;
									SeekMatchingBracket(&file);
									continue;
								}
							}
						}

						// @note: this might break for explicit nested struct members (anonymous struct)
						if(base_type_token.type == TokenType_::CloseSquigglyBracket) break;

						// Loop over comma-separated members here.
						bool in_comma_separated_list = true;
						while(in_comma_separated_list)
						{

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

							bool is_pointer = false;
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

							if(CheckNextTokenType_(&file, TokenType_::Identifier))
							{ // Member name (no current support for multi-identifier declarations, like const, volatile, etc.)
								Token member_name = NextToken(&file);

								if(TokenMatchesString(member_name, "const"))
								{
									member_name = NextToken(&file);
									member_is_const = true;
								}

								MetaType base_meta_type = MetaType::unknown;
								base_meta_type = InterpretTokenAsMetaType(base_type_token, is_pointer);

								char *base_type_format_specifier = "d";

								int array_dimension = 0;
								char yes_array[] = "[]";
								while(CheckNextTokenType_(&file, TokenType_::OpenSquareBracket))
								{ // Array member
									//pointer_character_string = yes_array;
									SeekMatchingBracket(&file);
									SkipTokens(&file, 1);
									++array_dimension;
								}

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
								Fmt(GLuint, u)
								Fmt(size_t, zu)
								else
								{
									known_meta_type = false;
								}

								char *const_string = "";
								if(member_is_const) const_string = "const ";

								if(array_dimension == 0 and known_meta_type)
								{
									fprintf(&out_file, "\tAppendCString(&string, \"  %.*s: %%%s (%s%.*s%s)\\n\", s->%.*s);\n\n",
											(int)member_name.length, member_name.start,
											base_type_format_specifier,
											const_string,
											(int)base_type_token.length, base_type_token.start,
											pointer_character_string,
											(int)member_name.length, member_name.start);
								}
								else if(array_dimension > 0)
								{
									fprintf(&out_file, "\tAppendCString(&string, \"  %.*s: %%p (%s%.*s",
											(int)member_name.length, member_name.start,
											const_string,
											(int)base_type_token.length, base_type_token.start);
									for(int i=0; i<array_dimension; i++)
									{
										fprintf(&out_file, "[]");
									}
									fprintf(&out_file, ")\\n\", s->%.*s);\n\n",
											(int)member_name.length, member_name.start);
								}
								else
								{ // Append the member as a string from a recursive MetaString() call, unless it's a pointer.
									if(is_pointer)
									{
										fprintf(&out_file, "\tAppendCString(&string, \"  %.*s: %%p (%s%.*s%s)\\n\", s->%.*s);\n\n",
												(int)member_name.length, member_name.start,
												const_string,
												(int)base_type_token.length, base_type_token.start,
												pointer_character_string,
												(int)member_name.length, member_name.start);
									}
									else
									{
										fprintf(&out_file, "\tAppendCString(&string, \"  %.*s: \");\n",
											    (int)member_name.length, member_name.start);
										fprintf(&out_file, "\tAppendString(&string, MetaString(&s->%.*s));\n",
												(int)member_name.length, member_name.start);
										fprintf(&out_file, "\tAppendCString(&string, \"(%s%.*s%s)\\n\");\n\n",
												const_string,
												(int)base_type_token.length, base_type_token.start,
												pointer_character_string);
									}
								}
							}

							if(CheckNextTokenType_(&file, TokenType_::Equals))
							{ // Default value initialization.
								SkipTokens(&file, 1);
								if(CheckNextTokenType_(&file, TokenType_::OpenSquigglyBracket))
								{ // Aggregate initializer.
									SeekMatchingBracket(&file);
									ConfirmTokenType_(&file, TokenType_::CloseSquigglyBracket);

								    in_comma_separated_list = false;
								    continue;
								}
								else
								{
									// @robustness: This will fail if there's a semi-colon inside
									// of a definition; e.g., char *a = ";;;;";
									SeekChar(&file, ';');
									in_comma_separated_list = false;
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

					fprintf(&out_file, "\tAppendCString(&string, \"}\");\n");
					fprintf(&out_file, "\n");
					fprintf(&out_file, "\treturn string;\n");
					fprintf(&out_file, "}\n");
				}
			}
		}

		FreeBuffer(&file);
		FindNextFileA(handle, &file_data);
	}

	FindClose(handle);

	FILE *real_out_file = fopen(root_dir "src/meta_print.cpp", "w");
	if(!real_out_file)
	{
		printf("Failed to open meta_print.cpp file");
		return 0;
	}
	fwrite(out_file.data, 1, out_file.p - out_file.data, real_out_file);
	fclose(real_out_file);

	FILE *real_header_file = fopen(root_dir "src/meta_print.h", "w");
	if(!real_header_file)
	{
		printf("Failed to open meta_print.h file");
		return 0;
	}
	fwrite(header_file.data, 1, header_file.p - header_file.data, real_header_file);
	fclose(real_header_file);

//	fclose(out_file);
//	fclose(header_file);



		// if(token.type == TokenType_::Identifier)
		// {

		// }

	// bool valid = true;
	// valid = valid && SeekNextLineThatBeginsWith(&file, "Introspect");
	// char *initial = file.p;
	// if(!valid)
	// {
	// 	printf("Invalid struct format: %.*s", int(file.p-initial), initial);
	// 	return 0;
	// }

	// Token struct_name_token = NextToken(&file);
	// printf("struct name: %.*s\n", (int)struct_name_token.length, struct_name_token.start);

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
	debug::timed_block_array_size = __COUNTER__;
}

TimedBlockEntry TIMED_BLOCK_ARRAY[__COUNTER__-1];