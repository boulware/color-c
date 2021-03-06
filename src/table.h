#ifndef TABLE_H
#define TABLE_H

//#define MemberOffset(type, member) ((size_t)&(((type*)0)->member))

// @note: I believe because of the way we allocate entries in the table, a 0-initialized
//        Id will always be an invalid index, because when we create a new entry,
//        we always increase the generation by 1, and the generation is initialized to 0
//        on table allocation. So... we probably don't need the NullIndex<>={-1,-1} business.
//	      AOWTC: we have a default constructor that just sets id=0, generation=0. I don't foresee
//        any issues with this, but keep it in mind.

template<typename Type>
struct Id
{
    int index;
    int generation;
};

struct Ability;
struct Breed;
struct Unit;

typedef Id<Ability> AbilityId;
typedef Id<Breed> BreedId;
typedef Id<Unit> UnitId;

template<typename Type>
struct TableEntry
{
    Type data;
    bool active;
    Id<Type> id;
    //int generation; // generation=0 implies entry has never been used (or the 32-bit counter has looped)
};

template<typename Type>
struct Table
{
    PoolId<Arena> arena_id;
	TableEntry<Type> *entries;  // Pointer to first entry inside .arena
    int entry_count;
    int max_entry_count;
};

template<typename Type>
TableEntry<Type>*
begin(Table<Type> &table);

template<typename Type>
TableEntry<Type>*
end(Table<Type> &table);

// Checks that two Id's refer to the same entry.
template <typename Type>
bool
operator==(Id<Type> a, Id<Type> b);

template<typename Type>
bool
operator!=(Id<Type> a, Id<Type> b);

template<typename Type> Table<Type> AllocTable(int max_entry_count, PoolId<Arena> arena_id);
template<typename Type> Id<Type>    CreateEntry(Table<Type> *table);
template<typename Type> Type *      GetEntryFromRawIndex(Table<Type> table, int index);
template<typename Type> Id<Type>    NullIndex();
template<typename Type> Type *      GetEntryFromId(Table<Type> table, Id<Type> id);
template<typename Type> void        DeleteEntry(Table<Type> *table, Id<Type> id);

// template <typename Type>
// Id<Type>
// GetGenerationalIndexFromIndex(Table<Type> table, int index);

#endif