#ifndef PLATFORM_H
#define PLATFORM_H

#include "types.h"
#include "strings.h"
#include "text_parsing.h"

typedef bool fnsig_LoadFileIntoFixedBufferAndNullTerminate(const char *, u8 *, u32);
typedef void *fnsig_LoadFileIntoMemory(const char*, u32*);
typedef bool fnsig_LoadTextFileIntoMemory(char*, char*, u32, u32*);
typedef bool fnsig_LoadFileIntoMemoryAndNullTerminate(const char *, char **, u32);
typedef void fnsig_WriteLineToFile(char*, char*);
typedef void fnsig_SwapBuffers();
typedef void fnsig_GetMousePos(int*, int*);
typedef void fnsig_SetMousePos(int, int);
typedef void fnsig_ExitGame();
typedef bool fnsig_WindowIsFocused();
typedef bool fnsig_WindowIsActive();
typedef void fnsig_CopyMemoryBlock(void*, void*, int);
typedef void fnsig_ShowCursor();
typedef void fnsig_HideCursor();
typedef	bool fnsig_wglSwapIntervalEXT(int);
typedef u32 fnsig_GetFileSize(const char*);
typedef bool fnsig_LoadFileIntoSizedBufferAndNullTerminate(const char *, Buffer *);
//typedef Utf32String fnsig_LoadUtf8FileAsUtf32String(const char *);

struct Platform {
	fnsig_LoadFileIntoFixedBufferAndNullTerminate *LoadFileIntoFixedBufferAndNullTerminate;
	fnsig_LoadFileIntoMemory *LoadFileIntoMemory;
	fnsig_LoadTextFileIntoMemory *LoadTextFileIntoMemory;
	fnsig_LoadFileIntoMemoryAndNullTerminate *LoadFileIntoMemoryAndNullTerminate;
	fnsig_WriteLineToFile *WriteLineToFile;
	fnsig_SwapBuffers *SwapBuffers;
	fnsig_GetMousePos *GetMousePos;
	fnsig_SetMousePos *SetMousePos;
	fnsig_ExitGame *ExitGame;
	fnsig_WindowIsFocused *WindowIsFocused;
	fnsig_WindowIsActive *WindowIsActive;
	fnsig_CopyMemoryBlock *CopyMemoryBlock;
	fnsig_ShowCursor *ShowCursor;
	fnsig_HideCursor *HideCursor;
	fnsig_wglSwapIntervalEXT *SwapIntervalEXT;
	fnsig_GetFileSize *GetFileSize;
	fnsig_LoadFileIntoSizedBufferAndNullTerminate *LoadFileIntoSizedBufferAndNullTerminate;
	//fnsig_LoadUtf8FileAsUtf32String *LoadUtf8FileAsUtf32String;
};

#endif