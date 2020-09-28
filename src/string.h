#ifndef STRING_H
#define STRING_H

// This is a comment
//#define Introspect

Introspect
struct String
{
	int length;
	int max_length;
	char *data;
};

char &CharAt(String *string, u32 index);

char *begin(String& string);
char *end(String& string);

bool StringEmpty(String string);
bool StringFull(String string);

bool AppendChar(String *string, char appended_char);
bool InsertChar(String *string, char inserted_char, int pos);
bool DeleteChar(String *string, int pos);
bool DeleteRange(String *string, int start, int end);

bool AppendCString(String *string, const char *c_string, ...);

String StringFromCString(const char *c_string, Arena *arena = &memory::per_frame_arena);
bool SubstringInString(String substring, String string);

void CopyFromCString(String *string, const char *c_string);

String CopyString(String src, Arena *arena = &memory::per_frame_arena);

int Utf8ToUtf32(String string, int index, u32 *utf32_char);

String LowerCase(String string, Arena *arena = &memory::per_frame_arena);

#endif