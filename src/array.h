#ifndef ARRAY_H
#define ARRAY_H

template<typename Type>
struct Array
{
	Type *data;    // Pointer to first element.
	Arena *arena;  // Pointer to arena to use for allocation.
	int count;     // Current number of entries.
	int max_count; // Max number of entries in currently allocated memory.
};

template<typename Type>
Type *begin(Array<Type> &array)
{
	return (array.data);
}

template<typename Type>
Type *end(Array<Type> &array)
{
	return (array.data + array.count);
}

template<typename Type>
void
operator+=(Array<Type> &array, Type entry);

template<typename Type>
void
ResizeArray(Array<Type> *array, int new_max_count);

template<typename Type>
Array<Type>
CreatePermanentArray(int allocated_size);

template<typename Type>
Array<Type>
CreateTempArray(int allocated_size);

#endif