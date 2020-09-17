#ifndef CSV_H
#define CSV_H

#include "util.h"

u32
NextUtf32Char(Buffer *buffer);

Utf32String
NextToken(Buffer *buffer);

#endif