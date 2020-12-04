#include "fixed_array.h"

template<typename Type>
Type &
Array<Type>::operator[](int index)
{
    Assert(data != nullptr);
    Assert(index >= 0 and index < this->count);

    return *(data + index);
}

template<typename Type>
Type *
begin(FixedArray<Type> &array)
{
    return (array.data);
}

template<typename Type>
Type *
end(FixedArray<Type> &array)
{
    return (array.data + array.count);
}

// Zero-initializes all elements of array.
template<typename Type>
void
ClearFixedArray(FixedArray<Type> *array)
{
    for(int i=0; i<array->count; ++i)
    {
        array[i] = {};
    }
}

template<typename Type>
void
AssignFixedArray(FixedArray<Type> *dst, FixedArray<Type> src)
{
    dst->data = src.data;
    dst->arena_id = src.arena_id;
    dst->count = src.count;
}

template<typename Type>
void
CopyFixedArray(FixedArray<Type> *dst, FixedArray<Type> src)
{
    Assert(dst.count == src.count);

    for(int i=0; i<m::Min(dst.count, src.count); ++i)
    {
        dst[i] = src[i];
    }
}

template<typename Type>
Array<Type>
CreateArrayFromArena(int max_count, PoolId<Arena> arena_id)
{

}

template<typename Type>
Array<Type>
CreateTempArray(int max_count)
{

}
