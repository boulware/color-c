#ifndef POOL_H
#define POOL_H

template<typename Type>
struct PoolId
{
    int value;
};

template<typename Type>
bool
operator==(PoolId<Type> a, PoolId<Type> b);

template<typename Type>
struct PoolEntry
{
    Type data;
    PoolId<Type> id;
};


template<typename Type>
struct Pool
{
    PoolEntry<Type> *entries;
    int entry_count;
    int max_entry_count;
    int id_counter;

    template<typename Type> Pool<Type> &operator=(Pool<Type> &other)
    {
        entries = other.entries;
        entry_count = other.entry_count;
        max_entry_count = other.max_entry_count;
        id_counter = other.id_counter;
    }
    template<typename Type> Pool<Type> volatile &operator=(Pool<Type> volatile &other)
    {
        entries = other.entries;
        entry_count = other.entry_count;
        max_entry_count = other.max_entry_count;
        id_counter = other.id_counter;
    }
};

template<typename Type>
Pool<Type>
AllocPool(int max_entry_count);

template<typename Type>
PoolId<Type>
CreateEntry(Pool<Type> *pool);

template<typename Type>
void
DeleteEntry(Pool<Type> *pool, PoolId<Type> id);

template<typename Type>
Type *
GetEntryFromId(Pool<Type> pool, PoolId<Type> id);

// Volatile methodes
#if 1
template<typename Type> PoolId<Type> CreateEntry(Pool<Type> volatile *pool);
template<typename Type> Type * GetEntryFromId(Pool<Type> volatile *pool, PoolId<Type> id);
template<typename Type> void DeleteEntry(Pool<Type> volatile *pool, PoolId<Type> id);
#endif

#endif