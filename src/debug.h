#ifndef DEBUG_H
#define DEBUG_H

#include <intrin.h>

namespace debug
{
	size_t timed_block_array_size;
	u64 start_count;
	u64 cycles_per_second;
}

struct TimedBlockEntry
{
	char *filename;
	char *function_name;
	int line_number;
	u64	hit_count;
	u64 total_cycle_count;
};

TimedBlockEntry TIMED_BLOCK_ARRAY[];

struct TimedBlock
{
	TimedBlockEntry *entry;
	u64 start_count;

	TimedBlock(int ID, char *filename, char *function_name, int line_number)
	{
		entry = TIMED_BLOCK_ARRAY + ID;
		entry->filename = filename;
		entry->function_name = function_name;
		entry->line_number = line_number;
		entry->hit_count += 1;
		start_count = __rdtsc();
	}

	~TimedBlock()
	{
		entry->total_cycle_count += __rdtsc()-start_count;
	}
};


#define TIMED_BLOCK TimedBlock PASTE(timed_block_entry, __LINE__)(__COUNTER__, __FILE__, __FUNCSIG__, __LINE__)

#endif