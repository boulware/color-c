#ifndef PLATFORM_H
#define PLATFORM_H

#include "types.h"
#include "text_parsing.h"

typedef bool fnsig_LoadFileIntoFixedBufferAndNullTerminate(const char *, u8 *, u32);
typedef void *fnsig_LoadFileIntoMemory(const char*, u32*);
typedef bool fnsig_LoadTextFileIntoMemory(char*, char*, u32, u32*);
typedef bool fnsig_LoadFileIntoMemoryAndNullTerminate(const char *, char **, u32);
typedef void fnsig_WriteLineToFile(char*, char*);
typedef void fnsig_SwapBuffers();
typedef void fnsig_GetMousePos(int*, int*);
typedef void fnsig_SetMousePos(int, int);
//typedef void fnsig_ExitGame();
typedef bool fnsig_WindowIsFocused();
typedef bool fnsig_WindowIsActive();
typedef void fnsig_CopyMemoryBlock(void*, void*, int);
typedef void fnsig_ShowCursor();
typedef void fnsig_HideCursor();
typedef	bool fnsig_SwapIntervalEXT(int);
typedef bool fnsig_GetFileSize(const char *, size_t *);
typedef bool fnsig_LoadFileIntoSizedBufferAndNullTerminate(const char *, Buffer *);
typedef void *fnsig_AllocateMemory(size_t size);
typedef s64 fnsig_PerformanceCounterFrequency();
//typedef Utf32String fnsig_LoadUtf8FileAsUtf32String(const char *);

struct Platform {
	#define mPlatformFunction(name) fnsig_##name *name;
	// e.g.,
	// MemberFunction(WriteLineToFile);
	// =>
	// fnsig_WriteLineToFile *WriteLineToFile;

	mPlatformFunction(LoadFileIntoFixedBufferAndNullTerminate);
	mPlatformFunction(LoadFileIntoMemory);
	mPlatformFunction(LoadTextFileIntoMemory);
	mPlatformFunction(LoadFileIntoMemoryAndNullTerminate);
	mPlatformFunction(WriteLineToFile);
	mPlatformFunction(SwapBuffers);
	mPlatformFunction(GetMousePos);
	mPlatformFunction(SetMousePos);
	//mPlatformFunction(ExitGame);
	mPlatformFunction(WindowIsFocused);
	mPlatformFunction(WindowIsActive);
	mPlatformFunction(CopyMemoryBlock);
	mPlatformFunction(ShowCursor);
	mPlatformFunction(HideCursor);
	mPlatformFunction(SwapIntervalEXT);
	mPlatformFunction(GetFileSize);
	mPlatformFunction(LoadFileIntoSizedBufferAndNullTerminate);
	mPlatformFunction(AllocateMemory);
	mPlatformFunction(PerformanceCounterFrequency);

	#undef mPlatformFunction
};

#endif