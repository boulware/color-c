#ifndef PLATFORM_H
#define PLATFORM_H

#include "types.h"
#include "text_parsing.h"
#include "work_entry.h"

struct WorkQueue;

typedef bool   fnsig_LoadFileIntoFixedBufferAndNullTerminate(const char *, u8 *, u32);
typedef void  *fnsig_LoadFileIntoMemory(const char*, u32*);
typedef bool   fnsig_LoadTextFileIntoMemory(char*, char*, u32, u32*);
typedef bool   fnsig_LoadFileIntoMemoryAndNullTerminate(const char *, char **, u32);
typedef void   fnsig_WriteLineToFile(char*, char*);
typedef void   fnsig_SwapBuffers();
typedef void   fnsig_GetMousePos(int*, int*);
typedef void   fnsig_SetMousePos(int, int);
typedef bool   fnsig_WindowIsFocused();
typedef bool   fnsig_WindowIsActive();
typedef void   fnsig_CopyMemoryBlock(void*, void*, int);
typedef void   fnsig_ShowCursor();
typedef void   fnsig_HideCursor();
typedef	bool   fnsig_SwapIntervalEXT(int);
typedef bool   fnsig_GetFileSize(const char *, size_t *);
typedef bool   fnsig_LoadFileIntoSizedBufferAndNullTerminate(const char *, Buffer *);
typedef void  *fnsig_AllocateMemory(size_t size);
typedef bool   fnsig_FreeMemory(void *);
typedef s64    fnsig_PerformanceCounterFrequency();
typedef void   fnsig_AnyKey();
typedef u32    fnsig_StartJob(void *);
typedef u32    fnsig_GetCallingThreadId();
typedef void   fnsig_CreateWorkQueue(WorkQueue **, int, char *);
typedef void   fnsig_AddWorkEntry(WorkQueue *, WorkEntry);
typedef void   fnsig_QueueReleaseSemaphore(WorkQueue *);
typedef size_t fnsig_WorkQueuePendingJobCount(WorkQueue *);
typedef void   fnsig_BlockAndTakeMutex(void *);
typedef void   fnsig_ReleaseMutex(void *);
typedef void   fnsig_ReadWriteBarrier();
//typedef Utf32String fnsig_LoadUtf8FileAsUtf32String(const char *);

typedef int fnsig_MyInterlockedIncrement(int volatile *);

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
	mPlatformFunction(FreeMemory);
	mPlatformFunction(PerformanceCounterFrequency);
	mPlatformFunction(AnyKey);
	mPlatformFunction(StartJob);
	mPlatformFunction(GetCallingThreadId);
	mPlatformFunction(MyInterlockedIncrement);
	mPlatformFunction(CreateWorkQueue);
	mPlatformFunction(AddWorkEntry);
	mPlatformFunction(QueueReleaseSemaphore);
	mPlatformFunction(WorkQueuePendingJobCount);

	mPlatformFunction(BlockAndTakeMutex);
	mPlatformFunction(ReleaseMutex);
	mPlatformFunction(ReadWriteBarrier);

	#undef mPlatformFunction
};



#endif