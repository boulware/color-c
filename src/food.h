#ifndef FOOD_H
#define FOOD_H

#include "strings.h"
#include "bitmap.h"

struct Food
{
	Utf32String en_singular;
	Utf32String en_plural;
	Utf32String de_singular;
	Utf32String de_plural;

	Bitmap bitmap;
};

#endif