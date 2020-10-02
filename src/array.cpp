#include "array."

template<typename Type>
void
operator+=(Array<Type> &array, Type entry)
{
	if(array.count >= array.max_count)
	{
		ResizeArray(&array, 2*array.max_count);
	}

	array.data[array.count++] = entry;
}

template<typename Type>
void
ResizeArray(Array<Type> *array, int new_max_count)
{
	if(new_max_count > array->max_count)
	{
		void *new_data = AllocFromArena(array->arena, sizeof(Type)*new_max_count);
		CopyMemoryBlock(array->data, new_data, sizeof(Type)*array->count);
	}
	else
	{
		array->count = m::Min(array->count, new_max_count);
	}

	array->max_count = new_max_count;
}

template<typename Type>
Array<Type>
CreatePermanentArray(int max_count)
{
	return Array<Type>{
		.data = (Type*)AllocFromArena(&memory::permanent_arena, sizeof(Type)*max_count),
		.arena = &memory::permanent_arena,
		.count = 0,
		.max_count = max_count
	};
}

template<typename Type>
Array<Type>
CreateTempArray(int max_count)
{
	return Array<Type>{
		.data = (Type*)AllocFromArena(&memory::per_frame_arena, sizeof(Type)*max_count),
		.arena = &memory::per_frame_arena,
		.count = 0,
		.max_count = max_count
	};
}