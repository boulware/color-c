#ifndef MEMORY_H
#define MEMORY_H

struct Arena
{
	void *start;
	void *end;
	void *current;
};

namespace memory
{
	const size_t arena_size = 1024*1024;
	Arena per_frame_arena;
	Arena permanent_arena;
};

Arena AllocateArena();
void ClearArena(Arena *arena);
size_t ArenaBytesRemaining(Arena arena);
char *ScratchString(int size);
void *AllocFromArena(Arena *arena, size_t byte_count, bool zero=false);
void *AllocTemp(size_t byte_count);
void *AllocPerma(size_t byte_count);

#endif