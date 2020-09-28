#include "meta_print(manual).h"

#include <string.h>
#include "utf32string.h"

String AsString(const Vec2f &o, int depth=1)
{
	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	String m = {};

	AppendCString(&string, "Vec2f {\n");

	// .x
	for(int i=0; i<depth; i++) AppendCString(&string, "  ");
	AppendCString(&string, "x: %f (float)\n", o.x);

	// .y
	for(int i=0; i<depth; i++) AppendCString(&string, "  ");
	AppendCString(&string, "y: %f (float)\n", o.y);

	for(int i=1; i<depth; i++) AppendCString(&string, "  ");
	AppendCString(&string, "}");

	return string;
}

String AsString(const Utf32String &o, int depth=1)
{
	// Utf32String {
	// 	length: [value] (int)
	// 	max_length: [value] (int)
	// 	data: [value] (u32*)
	// }
	String string = {};
	string.length = 0;
	string.max_length = 1024;
	string.data = ScratchString(string.max_length);

	String m = {};

	AppendCString(&string, "Utf32String {\n");
	AppendCString(&string, "  length: %d (int)\n", o.length);
	AppendCString(&string, "  max_length: %d (int)\n", o.max_length);
	AppendCString(&string, "  data: %p (u32 *)\n", o.data);
	m = AsString(o.pos, depth+1);
	AppendCString(&string, "  pos: %.*s\n", m.length, m.data);
	AppendCString(&string, "}");

	return string;
}
