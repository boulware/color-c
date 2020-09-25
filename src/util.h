#ifndef UTIL_H
#define UTIL_H

#include "types.h"

void CopyMemoryBlock(void *dst, void *src, int count);
size_t StringLength(const char *str);
bool CompareStrings(const char *a, const char *b);
bool CompareStringsStrict(const char *a, const char *b);
void CopyStringN_unsafe(char *dst, const char *src, size_t n);
char *TempFormatString(const char *fmt, va_list args);




// Creates a temporary string and formats it according to printf-style formatting rules.
// Use ONLY in a function with variadic arguments (e.g., void foo(int a, int b, ...);)
//
// Usage example:
//
// void foo(char *string, ...) {
// 		char *formatted_string;
// 		mFormatString(formatted_string, string);
// }
#define mFormatString(dst, fmt) \
	va_list args; \
	va_start(args, fmt); \
	dst = TempFormatString(fmt, args); \
	va_end(args);

#endif