#include "util.h"

#include "memory.h"

void
CopyMemoryBlock(void *dst, void *src, int count) {
	for(int i=0; i<count; i++)
	{
		((u8*)dst)[i] = ((u8*)src)[i];
	}
}

bool
CompareBytesN(const void *a, const void *b, size_t count)
{
	for(int i=0; i<count; i++)
	{
		if(*((u8*)a + i) != *((u8*)b + i)) return false;
	}

	return true;
}

size_t
StringLength(const char *str)
{
	size_t i = 0;
	while(str[i] != '\0') i++;
	return i;
}

bool
CompareStrings(const char *a, const char *b)
{
	if(*a == '\0' or *b == '\0')
	{
		// For now, we let empty strings be equal to each other,
		// but we don't allow this function to early out for empty strings,
		// because then "" == any arbitrary string
		if(*a == *b)
		{
			return true;
		}
		else return false;
	}
	// Compare strings, but if one is null-terminated before the other,
	// they will still be considered equal as long as all other bytes before
	// the null byte matched. See CompareStringsStrict() for a more strict comparison.

	// @bug: There's a buffer over-read bug here if either string either isn't null-terminated,
	// or doesn't own the null-terminator as part of its mapped memory. But if we're using C strings,
	// we should just always null-terminate everything anyway.

	size_t i = 0;
	while(a[i] != '\0' and b[i] != '\0')
	{
		if(a[i] != b[i]) {
			return false;
		}
		i += 1;
	}

	return true;
}

bool
CompareStringsStrict(const char *a, const char *b)
{
	// Compare strings, byte-for-byte. They must both be null-terminated
	// at the same location to be considered equal. See CompareStrings()
	// for a less strict comparison.

	size_t i = 0;
	while(a[i] != '\0' and b[i] != '\0')
	{
		if(a[i] != b[i]) {
			return false;
		}
		i += 1;
	}

	if(a[i] == b[i]) {
		return true;
	}
	else {
		return false;
	}
}

// @note: At the moment, this does not rely on *src being null-terminated, nor does it NT *dst.
// You must guarantee that *src has at least n bytes allocated before calling this
void
CopyStringN_unsafe(char *dst, const char *src, size_t n)
{
	for(size_t i=0; i<n; i++)
	{
		dst[i] = src[i];
	}
}

// You must guarantee that *src has at least n bytes allocated before calling this
void
CopyString(char *dst, const char *src, size_t n_max)
{
	size_t i;
	for(i=0; i<n_max; ++i)
	{
		dst[i] = src[i];
		if(src[i] == '\0') break;
	}

	if(i==n_max) dst[n_max-1] = '\0';
}

char *
TempFormatString(const char *fmt, va_list args)
{
	char *formatted_string = ScratchString(c::max_formatted_string_length);

	int formatted_length = vsprintf(formatted_string, fmt, args);
	if(formatted_length > c::max_formatted_string_length)
	{
		log(__FUNCTION__ " received a string longer (after applying formatting) "
			"than c::max_formatted_string_length (%d). "
			"The string is still formatted, but clipped to max formatted string length.",
			c::max_formatted_string_length);

		formatted_string[c::max_formatted_string_length-1] = '\0'; // vsprintf doesn't null append if the string is too long.
	}

	return formatted_string;
}