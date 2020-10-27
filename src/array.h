#ifndef ARRAY_H
#define ARRAY_H

#include <typeinfo>

NoImplementation
template<typename Type>
struct Array
{
	Type *data;              // Pointer to first element.
	PoolId<Arena> arena_id;  // Pointer to arena to use for allocation.
	int count;               // Current number of entries.
	int max_count;           // Max number of entries allowed. (This can be less than _allocated_count if an array is resized to a smaller value)
    int _allocated_count;    // Internal member, used to track how much is actually allocated.

	Type &operator[](int index);

    // Delete the copy constructor so that all user-level code must explicitly choose between
    // AssignArray() (points to same memory) and CopyArray() (points to different memory)
    template<typename Type> Array<Type> &operator=(Array<Type> &other) = delete;
    //template<typename Type> Array(const Array<Type> &other) = delete;
    //A(const A& other)
};

String
MetaString(float *s)
{
    String string = {};
    string.length = 0;
    string.max_length = 1024;
    string.data = ScratchString(string.max_length);

    AppendCString(&string, "%f", *s);

    return string;
}

template<typename Type>
String MetaString(const Array<Type> *s)
{
    String string = {};
    string.length = 0;
    string.max_length = 10000;
    string.data = ScratchString(string.max_length);

    AppendCString(&string, "[%d/%d: %s] {\n", s->count, s->max_count, typeid(Type).name());

    bool first = true;
    for(int i=0; i<s->count; ++i)
    {
        if(!first) {
            AppendCString(&string, ", ");
        }
        else first = false;

        AppendString(&string, MetaString(&s->data[i]));
    }
    //AppendCString(&string, "  count: %d (int)\n", s->count);

    AppendCString(&string, "}");

    return string;
}

template<typename Type> Type *      begin(Array<Type> &array);
template<typename Type> Type *      end(Array<Type> &array);
template<typename Type> bool        ValidArray(Array<Type> array);
template<typename Type> Type *      Append(Array<Type> *array, Type entry);
template<typename Type> Type *      AppendIfUnique(Array<Type> *array, Type entry);
template<typename Type> Type *      AppendEmptyElement(Array<Type> *array);
template<typename Type> Type *      operator+=(Array<Type> &array, Type entry);
template<typename Type> void        ResizeArray(Array<Type> *array, int new_max_count);
template<typename Type> bool        ElementInArray(Array<Type> *array, Type value);
template<typename Type> void        ClearArray(Array<Type> *array);
template<typename Type> void        AppendArrayToArray(Array<Type> *root_array, Array<Type> appended_array);
template<typename Type> void        AssignArray(Array<Type> *dst, Array<Type> src);
template<typename Type> void        CopyArray(Array<Type> *dst, Array<Type> src);
template<typename Type> Array<Type> CreateArrayFromArena(int max_count, PoolId<Arena> arena_id);
template<typename Type> Array<Type> CreatePermanentArray(int max_count);
template<typename Type> Array<Type> CreateTempArray(int max_count);

#endif