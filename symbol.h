#ifndef SYMBOLE_H_
#define SYMBOLE_H_

typedef struct { /*struct for the symbol table */
     char *symbol;
     char *property;
     int val;
} row_symbol;

typedef struct {
    row_symbol **rows;
    int size;
} symbol_table;

row_symbol* create_symbol_row(char *symbol, char *property, int *val);
void add_row_to_symbol_table(symbol_table *table_of_symbol ,row_symbol *row);
void print_symbol_table(symbol_table *table_of_symbol);
void macro_handler(symbol_table *table_of_symbol, char ** words, int num, int *has_error);
int is_exist_in_symbol_table(symbol_table *table_of_symbol, char *symbol);
int get_address(symbol_table *table_of_symbol, char *str);
void update_values(symbol_table *table_of_symbol,int *IC);
int is_extern(symbol_table *table_of_symbol, char *string);
void free_symbol_table(symbol_table *table);
symbol_table* new_symbol_table();
#endif