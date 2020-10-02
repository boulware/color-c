#include "table.h"

#include <typeinfo>

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
		log("[%s] Table couldn't create a requested entry because it was full.", typeid(Type).name());
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
GetEntryByIndex(Table<Type> table, int index)
{
	if(index < 0 or index >= table.entry_count) return nullptr;
	if(table.entries[index].active == false) return nullptr;

	return &table.entries[index].data;
}

template <typename Type>
Id<Type>
GetIndexByName(Table<Type> table, String search_string)
{
	Id<Type> table_id = NullIndex<Type>();

	for(auto &entry : table)
	{
		// This looks somewhat dangerous, but the templation will catch issues at
		// compile time, because Types that don't have a name member won't compile.
		if(CompareStrings(entry.data.name, search_string))
		{
			table_id = entry.id;
			break;
		}
	}

	return table_id;
}

template <typename Type>
Type *
GetEntryByName(Table<Type> table, String name)
{
	int index = GetIndexByName(table, name);
	if(index == -1) return nullptr;

	return
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