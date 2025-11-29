#include "database.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>


Table* table_create (const char *name, size_t row_size, int initial_capacity) 
{
	Table *table = (Table*) malloc(sizeof(Table));

	if (!table)
		return NULL;

	strncpy (table->name, name, MAX_TABLE_NAME - 1);
	table->name[MAX_TABLE_NAME - 1] = '\0';

	table->column_capacity = INITIAL_COLUMN_CAPACITY;
	table->column_count = 0;
	table->columns = (ColumnDef*) malloc
		(table->column_capacity * sizeof(ColumnDef));

	if (!table->rows)
	{
		free(table->columns);
		free(table);
		return NULL;
	}

	return table;
}

void table_destroy (Table *table)
{
	if (!table)
		return;

	if (table->columns)
		free(table->columns);

	if (table->rows)
		free(table->columns);

	free(table);
}

void table_clear (Table *table)
{
	if (!table)
		return;

	table->row_count = 0;
}

Table* table_clone (const Table* table)
{
	if (!table) 
		return NULL;

	Table *clone = table_create
		(table->name, table->row_size, table->row_capacity);

	if (!clone)
		return NULL;

	if (table->column_count > clone->column_capacity)
		table_reserve_rows(clone, table->column_capacity);

	clone->column_count = table->column_count;
	memcpy(clone->rows, table->rows, table->row_count * table->row_size);

	return clone;
}

int table_add_column (Table *table, const char *name, ColumnType type, size_t offset)
{
	if (!table || !name)
		return 0;

	// Resize column when needed
	if (table->column_count >= table->column_capacity)
	{
		int new_capacity = table->column_capacity * 2;
		ColumnDef* new_columns = (ColumnDef*) 
			realloc(table->columns, new_capacity * sizeof(ColumnDef));

		if (!new_columns)
			return 0;

		table->columns = new_columns;
		table->column_capacity = new_capacity;
	}

	ColumnDef* col = &table->columns[table->column_count++];
	strncpy(col->name, name, MAX_COLUMN_NAME - 1);
	col->name[MAX_COLUMN_NAME - 1] = '\0';
	col->type = type;
	col->offset = offset;

	return 1;
}

int table_remove_column (Table *table, const char *name)
{
	if (!table || !name)
		return 0;

	int index = table_get_column_index(table, name);

	if (index < 0)
		return  0;

	for (int j = index; j < table->column_count - 1; j++)
		table->columns[j] = table->columns[j + 1];

	table->column_count--;
	return 1;
}

int table_get_column_index (const Table* table, const char* name)
{
	if (!table || !name)
		return -1;

	int j = 0;

	while (j < table->column_count && 
		strcmp(table->columns[j].name, name) != 0)
		j++;

	if (j < table->column_count)
		return j;

	return -1;
}

ColumnDef* table_get_column (const Table* table, const char* name)
{
	int index = table_get_column_index(table, name);

	if (index < 0)
		return NULL;

	return &table->columns[index];
}

int table_insert (Table *table, const void *row)
{
	if (!table || !row)
		return -1;

	// resize and double the capcity 
	if (table->row_count >= table->row_capacity)
	{
		int new_capacity = table->row_capacity * 2;
		void *new_rows = realloc(table->rows, new_capacity * table->row_size);

		if (!new_rows)
			return -1;

		table->rows = new_rows;
		table->row_capacity = new_capacity;
	}

	void *dest = (char*)table->rows + (table->row_count * table->row_size);
	memcpy(dest, row, table->row_size);

	return table->row_count++;
}

int table_insert_many (Table *table, const void *rows, int count)
{
	if (!table || !rows || count <= 0)
		return -1;

	int needed_capacity = table->row_count + count;

	if (needed_capacity > table->row_capacity)
		table_reserve_rows(table, needed_capacity);

	int inserted = 0;
	const char *row_ptr = (const char*) rows;

	for (int j = 0; j < count; j++)
	{
		if (table_insert(table, row_ptr) >= 0)
			inserted++;

		row_ptr += table->row_size;
	}

	return inserted;
}
