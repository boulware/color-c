#ifndef UTIL_H
#define UTIL_H

#include "types.h"

void CopyMemoryBlock(void *dst, void *src, int count);
size_t StringLength(const char *str);
bool CompareStrings(const char *a, const char *b);
bool CompareStringsStrict(const char *a, const char *b);
void CopyStringN_unsafe(char *dst, const char *src, size_t n);
char *TempFormatString(const char *fmt, va_list args);

template <typename Type>
bool InRange(Type value, Type min, Type max);

#endif