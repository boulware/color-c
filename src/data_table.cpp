#include "data_table.h"

DataTable
AllocDataTable(int entry_size, int entry_max)
{
	DataTable table;
	table.data = (u8*)malloc(entry_size*entry_max);
	table.entry_size = entry_size;
	table.entry_count = 0;
	table.entry_max = entry_max;

	return table;
}

bool IsValidIndex(DataTable table, int index)
{
	return(index >= 0 and index < table.entry_count);
}

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

bool
GetEntryIndexByStringMember(DataTable table, size_t member_offset, const char *target, int *index)
{
	bool found_entry = false;

	for(int i=0; i<table.entry_count; i++)
	{
		char *member_string = *(char**)(table.data + i*table.entry_size + member_offset);
		if(CompareStrings(member_string, target))
		{
			found_entry = true;
			*index = i;
			break;
		}
	}

	return found_entry;
}

void *
GetEntryByIndex(DataTable table, int index)
{
	if(IsValidIndex(table, index))
	{
		return (void *)(table.data + table.entry_size*index);
	}
	else
	{
		return nullptr;
	}
}