#include "pool.h"

template<typename Type>
bool
operator==(PoolId<Type> a, PoolId<Type> b)
{
    return(a.value == b.value);
}

template<typename Type>
Pool<Type>
AllocPool(int max_entry_count)
{
    Pool<Type> pool = {};
    pool.entries = (PoolEntry<Type>*)platform->AllocateMemory(max_entry_count*sizeof(PoolEntry<Type>));
    pool.entry_count = 0;
    pool.max_entry_count = max_entry_count;
    pool.id_counter = 1; // 0 is reserved for invalid entries

    return pool;
}
/*
template<typename Type>
PoolId<Type>
CreateEntry(Pool<Type> *pool)
{
    if(pool->entry_count >= pool->max_entry_count)
    {
        // @unsure: does the mStringify macro happen before or after templatization?
        Log("[%s] Pool couldn't create a requested entry because it was full.", mStringify(Type));
        Assert(false);
        return PoolId<Type>{.value = 0};
    }

    PoolEntry<Type> &entry = pool->entries[pool->entry_count];

    entry.id.value = pool->id_counter++;

    ++pool->entry_count;
    return entry.id;
}
*/
template<typename Type>
PoolId<Type>
CreateEntry(Pool<Type> volatile *pool)
{
    if(pool->entry_count >= pool->max_entry_count)
    {
        // @unsure: does the mStringify macro happen before or after templatization?
        //Log("[%s] Pool couldn't create a requested entry because it was full.", mStringify(Type));
        Assert(false);
        return PoolId<Type>{.value = 0};
    }

    PoolEntry<Type> &entry = pool->entries[pool->entry_count];

    entry.id.value = pool->id_counter++;

    ++pool->entry_count;
    return entry.id;
}
/*
template<typename Type>
void
DeleteEntry(Pool<Type> *pool, PoolId<Type> id)
{
    if(id.value == 0) return; // invalid index. Do nothing.

    int pool_index = -1;
    for(int i=0; i<pool->entry_count; ++i)
    {
        PoolEntry<Type> &entry_to_be_deleted = pool->entries[i]; // alias
        if(id == entry_to_be_deleted.id)
        { // Found the entry to delete
            PoolEntry<Type> &last_entry_in_pool = pool->entries[pool->entry_count-1]; // alias
            entry_to_be_deleted = last_entry_in_pool; // Move last entry in pool to the deleted entry slot.
            --pool->entry_count;
            return;
            // Note: this should work even if the deleted entry was the last entry. It's redundant,
            //       because deleted_entry will be the same as last_entry_in_pool, but it should
            //       function logically. The alternative is to do a conditional beforehand, but
            //       I think we're better off without that. The redundant copy is probably faster.
        }
    }

    // Tried to delete an entry that was not in the pool.

    //Log(__FUNCTION__ "() tried to delete an entry that was not in the pool.");
    Assert(false);
}
*/
template<typename Type>
void
DeleteEntry(Pool<Type> volatile *pool, PoolId<Type> id)
{
    if(id.value == 0) return; // invalid index. Do nothing.

    int pool_index = -1;
    for(int i=0; i<pool->entry_count; ++i)
    {
        PoolEntry<Type> &entry_to_be_deleted = pool->entries[i]; // alias
        if(id == entry_to_be_deleted.id)
        { // Found the entry to delete
            PoolEntry<Type> &last_entry_in_pool = pool->entries[pool->entry_count-1]; // alias
            entry_to_be_deleted = last_entry_in_pool; // Move last entry in pool to the deleted entry slot.
            --pool->entry_count;
            return;
            // Note: this should work even if the deleted entry was the last entry. It's redundant,
            //       because deleted_entry will be the same as last_entry_in_pool, but it should
            //       function logically. The alternative is to do a conditional beforehand, but
            //       I think we're better off without that. The redundant copy is probably faster.
        }
    }

    // Tried to delete an entry that was not in the pool.

    //Log(__FUNCTION__ "() tried to delete an entry that was not in the pool.");
    Assert(false);
}


/*
template<typename Type>
Type *
GetEntryFromId(Pool<Type> pool, PoolId<Type> id)
{
    if(id.value == 0) return nullptr;

    for(int i=0; i<pool.entry_count; ++i)
    {
        PoolEntry<Type> &entry = pool.entries[i]; // alias
        if(id == entry.id) return &entry.data;
    }

    // An entry with the supplied id does not exist in the pool.
    return nullptr;
}
*/
#if 1
template<typename Type>
Type *
GetEntryFromId(Pool<Type> volatile *pool, PoolId<Type> id)
{
    if(id.value == 0) return nullptr;

    for(int i=0; i<pool->entry_count; ++i)
    {
        PoolEntry<Type> &entry = pool->entries[i]; // alias
        if(id == entry.id) return &entry.data;
    }

    // An entry with the supplied id does not exist in the pool.
    return nullptr;
}
#endif