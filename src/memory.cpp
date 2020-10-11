#include "memory.h"

Arena
AllocArena()
{
	Arena arena;
	arena.start = platform->AllocateMemory(memory::arena_size);
	arena.end = (u8*)arena.start + memory::arena_size;
	arena.current = arena.start;

	++game->number_of_arenas_allocated;

	return arena;
}

void
FreeArena(Arena *arena)
{
	if(!arena->start) return;
	platform->FreeMemory(arena->start);
}

void
ClearArena(Arena *arena)
{
	TIMED_BLOCK;

	#if !DEBUG_BUILD
		// Zero the arena if we're on debug build. This should make bugs where "cleared" temp
		// memory is references in future frames.
		u8 *p = (u8*)arena->start;
		size_t bytes_initialized = (u8*)arena->current-(u8*)arena->start;
		for(size_t i=0; i<bytes_initialized; i++)
		{
			p[i] = 0;
		}
	#endif
	arena->current = arena->start;

	arena->allocs_since_reset = 0;
}

size_t
ArenaBytesAllocated(Arena arena)
{
	return (u8*)arena.current - (u8*)arena.start;
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
		Log("ScratchString() tried to allocate a string larger than an arena. Ignoring request.");
		return nullptr;
	}

	if(ArenaBytesRemaining(memory::per_frame_arena) < size)
	{
#if 0
		Log("memory::per_frame_arena tried to allocate past its end. It's not large enough. \
To avoid overflow, we're resetting the pointer back to the beginning, but this will\
cause other scratch data to be overwritten before the frame ends.");
#endif

		memory::per_frame_arena.current = memory::per_frame_arena.start;
	}

	char *p = (char*)memory::per_frame_arena.current;
	memory::per_frame_arena.current = (u8*)memory::per_frame_arena.current + size;
	return p;
}

void *
AllocFromArena(Arena *arena, size_t byte_count, bool zero)
{
	if(byte_count > memory::arena_size)
	{
		Log(__FUNCTION__" tried to allocate a memory block larger than an arena. Ignoring request.");
		return nullptr;
	}

	if(ArenaBytesRemaining(*arena) < byte_count)
	{
		Log("CRITICAL ERROR: " __FUNCTION__ "() tried to allocate past end of permanent storage. We'll fall back on malloc() "
			"here in the release build just to be a bit error-resistant, but this is a serious error "
			"and we might see memory leaks as a result.");

		#if DEBUG_BUILD
			Assert(false);
		#else
			return malloc(byte_count);
		#endif
	}

	void *p = arena->current;
	arena->current = (u8*)arena->current + byte_count;

	if(zero)
	{
		for(int i=0; i<byte_count; i++)
		{
			*(((u8*)p)+i) = 0;
		}
	}

	++arena->allocs_since_reset;

	return p;
}

void *
AllocTemp(size_t byte_count)
{
	return AllocFromArena(&memory::per_frame_arena, byte_count, true);
}

void *
AllocPerma(size_t byte_count)
{
	return AllocFromArena(&memory::permanent_arena, byte_count);
}