#ifndef DATA_TABLE_H
#define DATA_TABLE_H

#define MemberOffset(type, member) ((size_t)&(((type*)0)->member))

struct DataTable
{
	u8 *data;			// Pointer to start of first entry
	int entry_size; 	// Size of each entry
	int entry_count; 	// # of valid entries currently in the table
	int entry_max;		// # of entries that currently allocated *data can hold
};

DataTable AllocDataTable(int entry_size, int entry_max);
bool IsValidIndex(DataTable table, int index);
void *CreateEntry(DataTable *table);
int DataTableEntriesRemaining(DataTable table);
bool GetEntryIndexByStringMember(DataTable table, int member_offset, const char *target, int *index);
void *GetEntryByIndex(DataTable table, int index);

#endif