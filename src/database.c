#include "database.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

Table* table_create (const char *name, size_t row_size, int initial_capacity)
{
	Table *table = (Table*) malloc(sizeof(Table));

	if (!table) 
		return NULL;
	
	strncpy(table->name, name, MAX_TABLE_NAME - 1);
	table->name[MAX_TABLE_NAME - 1] = '\0';

	table->column_capacity = INITIAL_COLUMN_CAPACITY;
	table->column_count = 0;
	table->columns = (ColumnDef*) malloc
		(table->column_capacity * sizeof(ColumnDef));

	if (!table->columns)
	{
		free(table);
		return NULL; 
	}

	table->row_size = row_size;
	table->row_capacity = initial_capacity > 0 ? 
		initial_capacity : INITIAL_ROW_CAPACITY;
	table->row_count = 0;
	table->rows = malloc(table->row_capacity * row_size);

	if (!table->rows)
	{
		free(table->columns);
		free(table);
		return  NULL; 
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
		free(table->rows);
	
	free(table);
}

void table_clear (Table *table)
{
	if (!table)
		return;

	table->row_count = 0;
}

Table* table_clone (const Table *table)
{
	if (!table)
		return NULL; 

	Table *clone = table_create(table->name, 
			table->row_size, table->row_capacity);

	if (!clone)
		return NULL;

	if (table->column_count > clone->column_capacity)
	{
		if (!table_reserve_columns(clone, table->column_count))
		{
			table_destroy(clone);
			return NULL;
		}
	}

	clone->column_count = table->column_count;
	memcpy(clone->columns, table->columns, table->column_count *
		sizeof(ColumnDef));
	clone->row_count = table->row_count;

	if (table->row_count > 0) 
		memcpy(clone->rows, table->rows, table->row_count * table->row_size); 

	return clone;
}

// Column Management
int table_add_column (Table *table, const char *name, 
					  ColumnType type, size_t offset)
{
	if (!table || !name)
		return 0;

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
		return 0;

	for (int j = index; j < table->column_count - 1; j++)
		table->columns[j] = table->columns[j + 1];

	table->column_count--;
	return 1;
}

int table_get_column_index (const Table *table, const char *name)
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

ColumnDef* table_get_column (const Table *table, const char *name)
{
	int index = table_get_column_index(table, name);

	if (index < 0)
		return NULL;

	return &table->columns[index];
}

// CRUD operations

// INSERT - returns index of inserted row, or -1 on error 
int table_insert(Table *table, const void *row);
int table_insert_many(Table *table, const void *rows, int count);

// SELECT - returns indices of matching rows 
int *table_select(const Table *table, PredicateFn predicate, void *user_data, int *result_count);

// SELECT ALL - returns new table with matching rows 
Table *table_select_all(const Table *table, PredicateFn predicate, void *user_data);

// UPDATE - returns number of rows updated 
int table_update(Table *table, PredicateFn predicate, MapFn mapper, void *user_data);

// DELETE - returns number of rows deleted 
int table_delete(Table *table, PredicateFn predicate, void *user_data);

// Query Operations 

void* table_get_row(const Table* table, int index);
void* table_find_first(const Table* table, PredicateFn predicate, void* user_data);
int table_count(const Table* table, PredicateFn predicate, void* user_data);
int table_exists(const Table* table, PredicateFn predicate, void* user_data);

// Sorting 

void table_sort(Table* table, CompareFn compare)
{
	if (!table || !compare || table->row_count <= 1)
	 return;

	qsort(table->rows, table->row_count, table->row_size, 
	   (int (*)(const void*, const void*))compare);
}

// Utility Functions 

void table_print(const Table* table);
int table_size(const Table* table);
int table_is_empty(const Table* table);

// Capacity Management 

int table_reserve_rows(Table* table, int capacity);
int table_reserve_columns(Table* table, int capacity);
int table_shrink_to_fit(Table* table);


