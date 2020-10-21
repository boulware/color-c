#include "array.h"

// @robustness: What are we supposed to do if the index is invalid, and the array
//              has no space allocated, so we can't just return the 0th element?
//              We could just point to some zero'd memory somewhere, but that sounds
//              very error-prone. Obviously in debug we can just crash or something,
//              but if we want "safe" behavior in a release version, we can't just
//              return the 0th element by default for an invalid index.
template<typename Type>
Type &
Array<Type>::operator[](int index)
{
	Assert(data != nullptr);

	return *(data + index);
}

template<typename Type>
bool
ValidArray(Array<Type> array)
{
	Arena *arena = GetArenaFromId(array.arena_id);
	if(!arena or !array.data) return false;

	return true;
}

template<typename Type>
Type *
Append(Array<Type> *array, Type value)
{
	Assert(array->data != nullptr);

	if(array->count >= array->max_count)
	{
		ResizeArray(array, 2*array->max_count);
	}

	Type *entry = &array->data[array->count++];
	*entry = value;
	return entry;
}

template<typename Type>
Type *
AppendEmptyElement(Array<Type> *array)
{
	Assert(array->data != nullptr);

	if(array->count >= array->max_count)
	{
		ResizeArray(array, 2*array->max_count);
	}

	Type *entry = &array->data[array->count++];
	ZeroMemoryBlock(entry, sizeof(Type));
	return entry;
}

template<typename Type>
bool
ElementInArray(Array<Type> *array, Type value)
{
	for(auto other : *array)
	{
		if(other == value) return true;
	}

	return false;
}

template<typename Type>
Type *
operator+=(Array<Type> &array, Type value)
{
	return Append(&array, value);
}

template<typename Type>
Type *
begin(Array<Type> &array)
{
	return (array.data);
}

template<typename Type>
Type *
end(Array<Type> &array)
{
	return (array.data + array.count);
}

// template<typename Type>
// Type &
// ElementAt(Array<Type> *array, int index)
// {
// 	if(index < 0 or index >= array->count)
// 	{
// 		//VerboseLog(__FUNCTION__"() got invalid index for Array<%s>", mStringify(Type));
// 		Assert(false);
// 		return array->data[0];
// 	}

// 	return array->data[index];
// }

template<typename Type>
void
ResizeArray(Array<Type> *array, int new_max_count)
{
	TIMED_BLOCK;

	if(new_max_count > array->_allocated_count)
	{
		void *new_data = AllocFromArena(array->arena_id, sizeof(Type)*new_max_count);
		CopyMemoryBlock(new_data, array->data, sizeof(Type)*array->count);
		array->data = (Type*)new_data;
		array->_allocated_count = new_max_count;
	}
	else
	{
		array->count = m::Min(array->count, new_max_count);
	}

	array->max_count = new_max_count;
}

template<typename Type>
void
AppendArrayToArray(Array<Type> *root_array, Array<Type> appended_array)
{
	int total_element_count = root_array->count + appended_array.count;
	if(total_element_count > root_array->max_count)
	{
		ResizeArray(root_array, total_element_count);
	}

	for(int i=0; i<appended_array.count; i++)
	{
		*root_array += appended_array[i];
	}
}

template<typename Type>
void
ClearArray(Array<Type> *array)
{
	array->count = 0;
}

template<typename Type>
Array<Type>
CreateArrayFromArena(int max_count, Id<Arena> arena_id)
{
	Arena *arena = GetArenaFromId(arena_id);
	if(!arena)
	{
		Log(__FUNCTION__ "() tried to allocate an array from an invalid arena.");
		Assert(false);
		return {};
	}

	return Array<Type>{
		.data = (Type*)AllocFromArena(arena_id, sizeof(Type)*max_count),
		.arena_id = arena_id,
		.count = 0,
		.max_count = max_count,
		._allocated_count = max_count
	};
}

template<typename Type>
Array<Type>
CreatePermanentArray(int max_count)
{
	return CreateArrayFromArena<Type>(max_count, memory::permanent_arena_id);
}

template<typename Type>
Array<Type>
CreateTempArray(int max_count)
{
	return CreateArrayFromArena<Type>(max_count, memory::per_frame_arena_id);
}

template<typename Type>
Array<Type>
AllocateArrayFromPlatform(int max_count)
{
	return Array<Type>{
		.data = platform->AllocateMemory(sizeof(Type) * max_count),
		.arena = nullptr,
		.count = 0,
		.max_count = max_count,
		._allocated_count = max_count,
	};
}