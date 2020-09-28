#include <string.h>
#include "utf32string.h"

String AsString(const Utf32String *s)
{
	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "Utf32String {\n");
	AppendCString(&string, "  length: %d (int)\n", s.length);
	AppendCString(&string, "  max_length: %d (int)\n", s.max_length);
	AppendCString(&string, "  data: %d (u32)\n", s.data);
	AppendCString(&string, "  pos: %d (Vec2f)\n", s.pos);
