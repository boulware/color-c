#ifndef BITMAP_H
#define BITMAP_H

#include "vec.h"
#include "sprite.h"

struct BgraPixel
{
	u8 b,g,r,a;
};

struct Bitmap
{
	u32 width, height;
	BgraPixel *pixels;
};

Bitmap AllocBitmap(u32 width, u32 height);
void DeallocBitmap(Bitmap *bitmap);
void Blit(Bitmap src, Bitmap dst, Vec2f pos);
//void FillBitmap(Bitmap target, Pixel color);
Bitmap LoadArgbBitmapFromFile(const char *filename);
Sprite LoadBitmapFileIntoSprite(const char *filename, Alignment align);
#endif