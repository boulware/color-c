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