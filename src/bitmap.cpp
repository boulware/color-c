#include "bitmap.h"

#include "math.h"
#include "util.h"
#include "sprite.h"

Bitmap
AllocBitmap(u32 width, u32 height)
{
	Bitmap bitmap;
	bitmap.width = width;
	bitmap.height = height;
	bitmap.pixels = (BgraPixel*)malloc(sizeof(BgraPixel)*width*height);

	return bitmap;
}

void
DeallocBitmap(Bitmap *bitmap)
{
	if(bitmap->pixels != nullptr)
	{
		free(bitmap->pixels);
	}

	*bitmap = {};
}

void
Blit(Bitmap src, Bitmap dst, Vec2i pos)
{
	// Copy row-by-row
	for(size_t y=0; y<src.height; y++)
	{
		for(size_t x=0; x<src.width; x++)
		{
			BgraPixel src_pixel = src.pixels[x + y*src.width];
			BgraPixel &dst_pixel = dst.pixels[(pos.x+x) + (pos.y+y)*dst.width];

			//if(src_pixel.a != 0) dst_pixel = src_pixel;
		}
	}
}

void
FillBitmap(Bitmap target, BgraPixel color)
{
	for(size_t i=0; i<target.width*target.height; i++)
	{
		target.pixels[i] = color;
	}
}

// Byte offsets into .bmp files
namespace bmp {
	static const int signature 		= 0x0;
	static const int filesize 		= 0x2;
	static const int data_offset 	= 0xA;
	static const int width 			= 0x12;
	static const int height 		= 0x16;
	static const int bpp 			= 0x1C;
	static const int compression	= 0x1E;

	#pragma pack(push, 1)
	struct Header
	{
		// "Header"
		char signature[2];
		u32 filesize;
		u32 reserved_header;
		u32 data_offset;

		// "InfoHeader"
		u32 infoheader_size;
		u32 bmp_width;
		u32 bmp_height;
		u16 planes;
		u16 bits_per_pixel;
		u32 compression;
		u32 image_size;
		u32 x_pixels_per_m;
		u32 y_pixels_per_m;
		u32 colors_used;
		u32 important_colors;
	};
	#pragma pack(pop)
}

Bitmap LoadArgbBitmapFromFile(const char *filename)
{
	Bitmap bitmap = {};

	//u32 buffer_size = 10000000;
	//u8 *buffer = (u8*)malloc(buffer_size);

	Buffer buffer;
	bool load_success = platform->LoadFileIntoSizedBufferAndNullTerminate(filename, &buffer);
	if(!load_success)
	{
		return bitmap;
	}

	bmp::Header header = *(bmp::Header*)buffer.data;
	if(!CompareBytesN(header.signature, "BM", 2))
	{
		Log("Invalid file format signature in bitmap file (\"%s\")", filename);
		goto FINAL;
	}
	if(header.bits_per_pixel != 32)
	{
		Log("Unsupported bmp file format (format: bpp %u, compression %u) (file: \"%s\")",
			header.bits_per_pixel, header.compression, filename);
		goto FINAL;
	}

	bitmap.width = header.bmp_width;
	bitmap.height = header.bmp_height;
	u32 data_offset = header.data_offset;

	u64 bytes_per_pixel = 4; // We've already guaranteed bits/pixel = 32
	u64 bytes_per_row = bitmap.width * bytes_per_pixel;

	bitmap.pixels = (BgraPixel*)malloc(4 * bitmap.width * bitmap.height);

	for(int i=0; i<bitmap.height; i++)
	{
		// BMPs are stored bottom to top, so we need (bitmap.height-1)-i to flip it when loading into memory.
		CopyMemoryBlock((u8*)bitmap.pixels+((bitmap.height-1)-i)*bytes_per_row, (u8*)buffer.data+data_offset+i*bytes_per_row, bytes_per_row);
	}

	FINAL:
		FreeBuffer(&buffer);
		return bitmap;
}

Sprite
LoadBitmapFileIntoSprite(const char *filename, Align align = c::align_topleft)
{
	Bitmap bitmap = LoadArgbBitmapFromFile(filename);

	Sprite sprite = {};
	sprite.texture = GenerateAndBindTexture();
	sprite.size = {(float)bitmap.width, (float)bitmap.height};
	if(align == c::align_center)
	{
		sprite.origin = 0.5f * sprite.size;
	}
	gl->TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap.width, bitmap.height,
				   0, GL_BGRA, GL_UNSIGNED_BYTE, bitmap.pixels);

	DeallocBitmap(&bitmap);
	return sprite;
}