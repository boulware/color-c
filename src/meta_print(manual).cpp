#include "meta_print(manual).h"

String MetaString(const u8 *s)
{
	TIMED_BLOCK;

	String string = {};
	string.length = 0;
	string.max_length = c::max_s32_digits;
	string.data = ScratchString(string.max_length);

	AppendCString(&string, "%u", *u8);

	return string;
}