#ifndef MEMORY_H
#define MEMORY_H

Introspect
struct Arena
{
	void *start;
	void *end;
	void *current;

    int allocs_since_reset;
};

namespace memory
{
	const size_t arena_size = 1024*1024;
	Arena per_frame_arena;
	Arena permanent_arena;
};

//int number_of_arenas_allocated = 0;

Arena *g_per_frame_arena = &memory::per_frame_arena;
Arena *g_permanent_arena = &memory::permanent_arena;

Arena AllocArena();
void FreeArena(Arena *arena);
void ClearArena(Arena *arena);
size_t ArenaBytesRemaining(Arena arena);
char *ScratchString(int size);
void *AllocFromArena(Arena *arena, size_t byte_count, bool zero=false);
void *AllocTemp(size_t byte_count);
void *AllocPerma(size_t byte_count);

#endif