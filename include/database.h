#ifndef DATABASE_H
#define DATABASE_H

#include <stddef.h>

#define MAX_COLUMN_NAME (100)
#define MAX_TABLE_NAME (100)
#define INITIAL_ROW_CAPACITY (100) 
#define INITIAL_COLUMN_CAPACITY (10)

// Structures 

enum column_type_tag
{
	COL_TYPE_INT,
	COL_TYPE_FLOAT,
	COL_TYPE_STRING,
	COL_TYPE_BOOL,
	COL_TYPE_DATE,
	COL_TYPE_TIME,
	COL_TYPE_DATETIME
};

typedef enum column_type_tag ColumnType;

struct column_def_tag
{
	char name[MAX_COLUMN_NAME];
	ColumnType type;
	size_t offset;
};

typedef struct column_def_tag ColumnDef;

struct table_tag
{
	char name[MAX_TABLE_NAME];

	ColumnDef* columns;
	int column_count;
	int column_capacity;

	void* rows;
	int row_count;
	int row_capacity;
	size_t row_size;
};

typedef struct table_tag Table;

// Function Pointers

typedef int (*PredicateFn) (const void *row, const void *col);
typedef int (*CompareFn) (const void *a, const void *b);
typedef void (*MapFn) (void *row, void *user_data);

// Table Functions
Table* table_create (const char *name, size_t row_size, int initial_capacity);
void table_destroy (Table *table);
void table_clear (Table *table);
Table* table_clone (const Table *table);

// Column Management
int table_add_column (Table *table, const char *name, ColumnType type, size_t offset);
int table_remove_column (Table *table, const char *name);
int table_get_column_index (const Table *table, const char *name);
ColumnDef* table_get_column (const Table *table, const char *name);

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

void table_sort(Table* table, CompareFn compare);

// Utility Functions 

void table_print(const Table* table);
int table_size(const Table* table);
int table_is_empty(const Table* table);

// Capacity Management 

int table_reserve_rows(Table* table, int capacity);
int table_reserve_columns(Table* table, int capacity);
int table_shrink_to_fit(Table* table);

// Helper Macros 

#define OFFSETOF(type, member) ((size_t)&((type*)0)->member)
#define TABLE_ROW(table, index, type) ((type*)table_get_row(table, index))

#endif
