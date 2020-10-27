#ifndef MEMORY_H
#define MEMORY_H

#include "pool.h"

Introspect
struct Arena
{
	void *start;
	void *end;
	void *current;

    int allocs_since_reset;
    char debug_name[64];
    void *max_current; // The highest *current has ever been.
};

namespace memory
{
	const size_t arena_size = 1024*1024;
    void *arena_pool_mutex_handle;
    PoolId<Arena> default_arena_id;
	PoolId<Arena> per_frame_arena_id;
	PoolId<Arena> permanent_arena_id;
};


// Temporary to get around that RemedyBG does not support watching namespaced variables
PoolId<Arena> *g_per_frame_arena_id = &memory::per_frame_arena_id;
PoolId<Arena> *g_permanent_arena_id = &memory::permanent_arena_id;

PoolId<Arena> AllocArena(char *debug_name);
void FreeArena(PoolId<Arena> arena_id);
void ClearArena(PoolId<Arena> arena_id);
size_t ArenaBytesRemaining(PoolId<Arena> arena_id);
char *ScratchString(int size);
void *AllocFromArena(PoolId<Arena> arena_id, size_t byte_count, bool zero=false);
void *AllocTemp(size_t byte_count);
void *AllocPerma(size_t byte_count);

#endif