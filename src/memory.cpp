#include "memory.h"

Arena
AllocateArena()
{
	Arena arena;
	arena.start = platform->AllocateMemory(memory::arena_size);
	arena.end = (u8*)arena.start + memory::arena_size;
	arena.current = arena.start;

	return arena;
}

void
ClearArena(Arena *arena)
{
	TIMED_BLOCK;
	arena->current = arena->start;
}

size_t
ArenaBytesRemaining(Arena arena)
{
	return (u8*)arena.end - (u8*)arena.current;
}

char *
ScratchString(int size)
{
	TIMED_BLOCK;
	if(size > memory::arena_size)
	{
		log("ScratchString() tried to allocate a string larger than an arena. Ignoring request.");
		return nullptr;
	}

	if(ArenaBytesRemaining(memory::per_frame_arena) < size)
	{
		log("memory::per_frame_arena tried to allocate past its end. It's not large enough. \
To avoid overflow, we're resetting the pointer back to the beginning, but this will\
cause other scratch data to be overwritten before the frame ends.");

		memory::per_frame_arena.current = memory::per_frame_arena.start;
	}

	char *p = (char*)memory::per_frame_arena.current;
	memory::per_frame_arena.current = (u8*)memory::per_frame_arena.current + size;
	return p;
}