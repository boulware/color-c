#include "image.h"

#include "types.h"

// Byte offsets specified in PNG spec
namespace png
{
	// Offsets relative to beginning of file.
	const size_t offset_header = 0;

	// Offsets relative to beginning of chunk.
	const size_t offset_chunk_length	= 0;
	const size_t offset_chunk_type		= 4;
	const size_t offset_chunk_data		= 8;

	// Misc.
	const size_t chunk_preamble_size = 8;
	const size_t crc_size = 4;
	const size_t minimum_chunk_size = 8;

	struct Chunk
	{
		u32 length;
		char type[4];
		void *data;
	};

	struct IHDR
	{
		u32 width, height;
		u8 bit_depth;
		u8 color_type;
		u8 compression_method;
		u8 filter_method;
		u8 interlace_method;
	};
	// // IHDR chunk data offsets
	// const size_t ihdr_width			= 0;
	// const size_t ihdr_height		= 4;
	// const size_t ihdr_depth			= 8;
	// const size_t ihdr_colortype		= 9;
	// const size_t ihdr_compression	= 10;
	// const size_t ihdr_filter		= 11;
	// const size_t ihdr_interlace		= 12;
};

png::Chunk
ReadChunk(void *start)
{
	png::Chunk chunk;
	chunk.length = ByteSwapU32(*(u32*)((u8*)start + png::offset_chunk_length));
	CopyMemoryBlock(&chunk.type, (u8*)start + png::offset_chunk_type, 4);
	chunk.data = (void*)((u8*)start + png::offset_chunk_data);

	return chunk;
}

void
ByteSwapIHDR(png::IHDR *chunk_data)
{
	chunk_data->width = ByteSwapU32(chunk_data->width);
	chunk_data->height = ByteSwapU32(chunk_data->height);
}

bool
LoadPngFileIntoTexture(const char *filename, GLuint *texture)
{
	bool success = true;

	Buffer file;
	bool fileload_success = platform->LoadFileIntoSizedBufferAndNullTerminate(filename, &file);
	if(!fileload_success) return nullptr;

	u8 png_header[] = {137, 80, 78, 71, 13, 10, 26, 10};

	bool header_ok = CompareBytesN(file.data, png_header, sizeof(png_header));
	if(!header_ok)
	{
		log("LoadPngFromFile() was given a file with an invalid PNG file header.");
		success = false;
		goto FINAL;
	}

	// Advance past header to beginning of first chunk.
	file.p += sizeof(png_header);

	png::Chunk chunk;

	// IHDR chunk
	chunk = ReadChunk(file.p);
	png::IHDR ihdr = *(png::IHDR*)chunk.data;
	ByteSwapIHDR(&ihdr);
	if(!CompareBytesN(chunk.type, "IHDR", 4))
	{
		log("First PNG chunk was not IHDR (file: \"%s\")", filename);
		success = false;
		goto FINAL;
	}

	// Checking if the PNG is a supported format for this simple decoder.
	if(	ihdr.bit_depth != 8 or
		ihdr.color_type != 6 or
		ihdr.compression_method != 0 or
		ihdr.filter_method != 0 or
		ihdr.interlace_method != 0)
	{
		log("PNG decoder doesn't support format (format: color_type %u, compression %u, filter %u, interlace %u) (file: \"%s\")",
			ihdr.color_type, ihdr.compression_method, ihdr.filter_method, ihdr.interlace_method, filename);
		success = false;
		goto FINAL;
	}

	// Advance past IHDR to the beginning of next chunk.
	file.p += png::chunk_preamble_size + chunk.length + png::crc_size;

	bool IEND_reached = false;
	while(!IEND_reached and BufferBytesRemaining(file) >= png::minimum_chunk_size)
	{
		chunk = ReadChunk(file.p);
		if(CompareBytesN(chunk.type, "IDAT", 4))
		{
			// width*height pixels, 4 bytes per pixel, 1 filter byte per row
			u32 expected_byte_count = 4 * ihdr.width * ihdr.height + ihdr.height;
			log("IDAT size: %u (expected: %u)", chunk.length, expected_byte_count);
			u8 *pixels = (u8*)chunk.data + 1; // Skip filter byte
		}
		else if(CompareBytesN(chunk.type, "IEND", 4))
		{
			IEND_reached = true;
		}
		else
		{
			log("Skipping unsupported chunk type (%.4s) in PNG file \"%s\"", chunk.type, filename);
		}

		file.p += png::chunk_preamble_size + chunk.length + png::crc_size;
	}

	FINAL:
		FreeBuffer(&file);
		return success;
}