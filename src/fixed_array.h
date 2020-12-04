#ifndef FIXED_ARRAY_H
#define FIXED_ARRAY_H

NoImplementation
template<typename Type>
struct FixedArray
{
    Type *data;
    PoolId<Arena> arena_id;
    int count;

    Type &operator[](int index);
};

template<typename Type> Type *      begin(FixedArray<Type> &array);
template<typename Type> Type *      end(FixedArray<Type> &array);
template<typename Type> void        AssignFixedArray(FixedArray<Type> *dst, FixedArray<Type> src);
template<typename Type> void        CopyFixedArray(FixedArray<Type> *dst, FixedArray<Type> src);
template<typename Type> Array<Type> CreateArrayFromArena(int max_count, PoolId<Arena> arena_id);
template<typename Type> Array<Type> CreateTempArray(int max_count);

#endif