#include "memory.h"

PoolId<Arena>
AllocArena(char *debug_name)
{
    platform->BlockAndTakeMutex(memory::arena_pool_mutex_handle);
        PoolId<Arena> arena_id = CreateEntry(game->arena_pool);
        platform->ReadWriteBarrier();
    platform->ReleaseMutex(memory::arena_pool_mutex_handle);

    Arena *arena = GetEntryFromId(game->arena_pool, arena_id);
    if(!arena)
    {
        //Log(__FUNCTION__ "() failed to get a valid arena_id. Mutex issue?");
        Assert(false);
        return c::null_arena_id;
    }


    arena->start = platform->AllocateMemory(memory::arena_size);
    arena->end = (u8*)arena->start + memory::arena_size;
    arena->current = arena->start;
    arena->allocs_since_reset = 0;
    arena->max_current = arena->start;

    if(debug_name)
    {
        CopyCString(arena->debug_name, debug_name, ArrayCount(arena->debug_name));
    }
    else
    {
        arena->debug_name[0] = '\0';
    }

    return arena_id;
}

Arena *
GetArenaFromId(PoolId<Arena> arena_id)
{
    return GetEntryFromId(game->arena_pool, arena_id);
}

// Arena *
// GetArenaFromId(PoolId<Arena> volatile arena_id)
// {
//     return GetEntryFromId(*game->arena_pool, *arena_id);
// }

void
FreeArena(PoolId<Arena> arena_id)
{
    platform->BlockAndTakeMutex(memory::arena_pool_mutex_handle);

    Arena *arena = GetArenaFromId(arena_id);
    if(arena and arena->start)
    {
        platform->FreeMemory(arena->start);
    }

    DeleteEntry(game->arena_pool, arena_id);

    platform->ReadWriteBarrier();
    platform->ReleaseMutex(memory::arena_pool_mutex_handle);
}

void
ClearArena(PoolId<Arena> arena_id)
{
    TIMED_BLOCK;

    Arena *arena = GetArenaFromId(arena_id);
    if(!arena) return;

    //#if DEBUG_BUILD
    #if 0
        // Zero the arena if we're on debug build. This should make bugs where "cleared" temp
        // memory is references in future frames.
        u8 *p = (u8*)arena->start;
        size_t bytes_initialized = (u8*)arena->current-(u8*)arena->start;
        for(size_t i=0; i<bytes_initialized; i++)
        {
            p[i] = 0;
        }
    #endif
    arena->max_current = m::Max(arena->current, arena->max_current);
    arena->current = arena->start;

    arena->allocs_since_reset = 0;
}

size_t
ArenaBytesAllocated(PoolId<Arena> arena_id)
{
    Arena *arena = GetArenaFromId(arena_id);
    if(!arena) return 0;

    return (u8*)arena->current - (u8*)arena->start;
}

size_t
ArenaBytesRemaining(PoolId<Arena> arena_id)
{
    Arena *arena = GetArenaFromId(arena_id);
    if(!arena) return 0;

    return (u8*)arena->end - (u8*)arena->current;
}

void *
AllocFromArena(PoolId<Arena> arena_id, size_t byte_count, bool zero)
{
    Arena *arena = GetArenaFromId(arena_id);
    if(!arena)
    {
        Log(__FUNCTION__ "() tried to allocate from invalid arena. Falling back to malloc() in release build.");

        #if DEBUG_BUILD
            Assert(false);
            return nullptr;
        #else
            return malloc(byte_count);
        #endif
    }

    if(byte_count > memory::arena_size)
    {
        Log(__FUNCTION__" tried to allocate a memory block larger than an arena. Ignoring request.");
        return nullptr;
    }

    if(ArenaBytesRemaining(arena_id) < byte_count)
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
    return AllocFromArena(memory::per_frame_arena_id, byte_count, true);
}

void *
AllocPerma(size_t byte_count)
{
    return AllocFromArena(memory::permanent_arena_id, byte_count);
}

char *
ScratchString(int size)
{
    TIMED_BLOCK;

    return (char *)AllocTemp(size);

    // if(size > memory::arena_size)
    // {
    //     Log("ScratchString() tried to allocate a string larger than an arena. Ignoring request.");
    //     return nullptr;
    // }

    // Arena *scratch_arena = GetArenaFromId(memory::per_frame_arena_id);
    // if(!scratch_arena)
    // {
    //     Log(__FUNCTION__ "() called, but scratch arena doesn't exist.");
    //     return nullptr;
    // }

    // if(ArenaBytesRemaining(memory::per_frame_arena_id) < size)
    // {
    //     scratch_arena->current = scratch_arena->start;
    // }

    // char *p = (char*)(scratch_arena->current);
    // scratch_arena->current = (u8*)(scratch_arena->current) + size;
    // return p;
}