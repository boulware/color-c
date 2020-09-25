#ifndef DATA_TABLE_H
#define DATA_TABLE_H

#define MemberOffset(type, member) ((size_t)&(((type*)0)->member))

struct DataTable
{
	u8 *data;			// Pointer to start of first entry
	int entry_size; 	// Size of each entry
	int entry_count; 	// # of valid entries currently in the table
	int entry_max;		// # of entries that currently allocated *data can hold

	void *operator[](int index);
};

DataTable AllocDataTable(int entry_size, int entry_max);
bool IsValidIndex(DataTable table, int index);
void *CreateEntry(DataTable *table);
int DataTableEntriesRemaining(DataTable table);
int GetEntryIndexByStringMember(DataTable table, size_t member_offset, const char *target);
void *GetEntryByIndex(DataTable table, int index);

template <typename Type>
int GetIndexByName(DataTable table, const char *entry_name);

template <typename Type>
Type *GetEntryByName(DataTable table, const char *entry_name);

#endif