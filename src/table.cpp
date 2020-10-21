#include "table.h"

template<typename Type>
TableEntry<Type>*
begin(Table<Type> &table)
{
	return (table.entries);
}

template<typename Type>
TableEntry<Type>*
end(Table<Type> &table)
{
	return (table.entries + table.entry_count);
}

template<typename Type>
Id<Type>
CreateId(int id, int generation)
{
	return Id<Type> {.id = id, .generation = generation};
}

template<typename Type>
bool operator==(Id<Type> a, Id<Type> b)
{
	return(a.index == b.index and a.generation == b.generation);
}

template<typename Type>
bool
operator!=(Id<Type> a, Id<Type> b)
{
	return(!(a==b));
}

template<typename Type>
Table<Type>
AllocTable(int max_entry_count)
{
	Table<Type> table = {};
	table.entries = (TableEntry<Type>*)platform->AllocateMemory(max_entry_count*sizeof(TableEntry<Type>));
	table.entry_count = 0;
	table.max_entry_count = max_entry_count;

	return table;
}

// Optional argument [new_entry] will be filled with a pointer to the new entry if
// it's not a nullptr.
template<typename Type>
Id<Type>
CreateEntry(Table<Type> *table)
{
	if(table->entry_count >= table->max_entry_count)
	{
		// @unsure: does the mStringify macro happen before or after templatization?
		Log("[%s] Table couldn't create a requested entry because it was full.", mStringify(Type));
		return Id<Type>{.index = 0, .generation = 0};
	}

	TableEntry<Type> &entry = table->entries[table->entry_count];
	Assert(entry.active == false);

	entry.active = true;
	entry.id.index = table->entry_count;
	entry.id.generation += 1;

	++table->entry_count;
	return entry.id;
}

template <typename Type>
Type *
GetEntryFromRawIndex(Table<Type> table, int index)
{
	if(index < 0 or index >= table.entry_count) return nullptr;
	if(table.entries[index].active == false) return nullptr;

	return &table.entries[index].data;
}

template <typename Type>
Id<Type>
NullIndex()
{
	return Id<Type>{-1,-1};
}

template <typename Type>
Type *
GetEntryFromId(Table<Type> table, Id<Type> id)
{
	if(id.index < 0 or id.index >= table.entry_count) return nullptr;

	auto &entry = table.entries[id.index];
	if(entry.id != id) return nullptr;

	return &entry.data;
}

template<typename Type>
void
DeleteEntry(Table<Type> *table, Id<Type> id)
{
	if(id.index < 0 or id.index >= table->entry_count) return; // Not a valid index (and thus id)

	auto &entry = table->entries[id.index];
	if(entry.id != id) return; // Outdated or invalid id, so don't delete the entry at this index.

	entry.active = false; // Setting an entry to inactive is equivalent to deleting it, since any
	                      // access with the current id will just return a nullptr
}

// template <typename Type>
// Id<Type>
// GetGenerationalIndexFromIndex(Table<Type> table, int index)
// {
// 	if(index < 0 or index >= table.entry_count) return NullIndex<Type>();

// 	Id<Type> table_index;
// 	table_index.index = index;
// 	table_index.generation = table.entries[index].generation;

// 	return table_index;
// }