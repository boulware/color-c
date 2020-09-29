#include "data_table.h"

void *
DataTable::operator[](int index)
{
	if(IsValidIndex(*this, index))
	{
		return (data + entry_size*index);
	}
	else
	{
		return nullptr;
	}
}

DataTable
AllocDataTable(int entry_size, int entry_max)
{
	DataTable table;
	table.data = (u8*)calloc(entry_size*entry_max, 1);
	table.entry_size = entry_size;
	table.entry_count = 0;
	table.entry_max = entry_max;

	return table;
}

bool IsValidIndex(DataTable table, int index)
{
	return(index >= 0 and index < table.entry_count);
}

// If there's not enough room in the DataTable, returns nullptr.
// Otherwise, returns pointer to the newly created entry.
void *
CreateEntry(DataTable *table)
{
	if(table->entry_count+1 > table->entry_max)
	{
		return nullptr;
	}

	void *new_entry = (void *)(table->data + table->entry_size*table->entry_count);
	table->entry_count += 1;
	return new_entry;
}

int DataTableEntriesRemaining(DataTable table)
{
	return(table.entry_max - table.entry_count);
}

// Returns -1 if no entry's member matches the given string.
// int
// GetEntryIndexByStringMember(DataTable table, size_t member_offset, const char *target)
// {
// 	int index = -1;

// 	for(int i=0; i<table.entry_count; i++)
// 	{
// 		char *member_string = (char*)(table.data + i*table.entry_size + member_offset);
// 		if(CompareStrings(member_string, target))
// 		{
// 			index = i;
// 			break;
// 		}
// 	}

// 	return index;
// }

// void *
// GetEntryByIndex(DataTable table, int index)
// {
// 	if(IsValidIndex(table, index))
// 	{
// 		return (void *)(table.data + table.entry_size*index);
// 	}
// 	else
// 	{
// 		return nullptr;
// 	}
// }

// template <typename Type>
// int GetIndexByName(DataTable table, const char *entry_name)
// {
// 	return GetEntryIndexByStringMember(table, MemberOffset(Type,name), entry_name);
// }

template <typename Type>
int
GetIndexByName(DataTable table, String search_string)
{
	int index = -1;

	for(int i=0; i<table.entry_count; i++)
	{
		String member_string = ((Type*)(table.data + i*table.entry_size))->name;
		if(CompareStrings(member_string, search_string))
		{
			index = i;
			break;
		}
	}

	return index;
}

template <typename Type>
Type *GetEntryByName(DataTable table, const char *entry_name)
{
	return (Type*)table[GetIndexByName<Type>(table, entry_name)];
}