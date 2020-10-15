#ifndef ARRAY_H
#define ARRAY_H

template<typename Type>
struct Array
{
	Type *data;    // Pointer to first element.
	Arena *arena;  // Pointer to arena to use for allocation.
	int count;     // Current number of entries.
	int max_count; // Max number of entries allowed. (This can be less than _allocated_count if an array is resized to a smaller value)
    int _allocated_count; // Internal member, used to track how much is actually allocated.

	Type &operator[](int index);
};

template<typename Type>
Type *
begin(Array<Type> &array);

template<typename Type>
Type *
end(Array<Type> &array);

template<typename Type>
bool
ValidArray(Array<Type> array);

template<typename Type>
Type *
Append(Array<Type> *array, Type entry);

template<typename Type>
Type *
AppendEmptyElement(Array<Type> *array);

template<typename Type>
Type *
operator+=(Array<Type> &array, Type entry);

template<typename Type>
void
ResizeArray(Array<Type> *array, int new_max_count);

template<typename Type>
bool
ElementInArray(Array<Type> *array, Type value);

template<typename Type>
void
ClearArray(Array<Type> *array);

template<typename Type>
void
AppendArrayToArray(Array<Type> *root_array, Array<Type> appended_array);

template<typename Type>
Array<Type>
CreateArrayFromArena(int max_count, Arena *arena);

template<typename Type>
Array<Type>
CreatePermanentArray(int max_count);

template<typename Type>
Array<Type>
CreateTempArray(int max_count);

template<typename Type>
Array<Type>
AllocateArrayFromPlatform(int max_count);


#endif