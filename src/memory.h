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
};

Arena AllocateArena();
void ClearArena(Arena *arena);
char *ScratchString(int size);

#endif