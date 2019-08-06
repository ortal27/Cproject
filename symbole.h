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
void on_macro(symbol_table *table_of_symbol, char ** words);
int is_exist_in_symbol_table(symbol_table *table_of_symbol, char *symbol);
void exit_with_message(char *symbol);
int symbol_address(symbol_table *table_of_symbol, char *str);
void change_val(symbol_table *table_of_symbol,int *IC);
int is_extern(symbol_table *table_of_symbol, char *string);
#endif