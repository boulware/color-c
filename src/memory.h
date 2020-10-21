#ifndef MEMORY_H
#define MEMORY_H

#include "table.h"

Introspect
struct Arena
{
	void *start;
	void *end;
	void *current;

    int allocs_since_reset;
    char debug_name[64];
};

namespace memory
{
	const size_t arena_size = 1024*1024;
	Id<Arena> permanent_arena_id;
	Id<Arena> per_frame_arena_id;
};


// Temporary to get around that RemedyBG does not support watching namespaced variables
Id<Arena> *g_per_frame_arena_id = &memory::per_frame_arena_id;
Id<Arena> *g_permanent_arena_id = &memory::permanent_arena_id;

Id<Arena> AllocArena(char *debug_name);
void FreeArena(Id<Arena> arena_id);
void ClearArena(Id<Arena> arena_id);
size_t ArenaBytesRemaining(Id<Arena> arena_id);
char *ScratchString(int size);
void *AllocFromArena(Id<Arena> arena_id, size_t byte_count, bool zero=false);
void *AllocTemp(size_t byte_count);
void *AllocPerma(size_t byte_count);

#endif