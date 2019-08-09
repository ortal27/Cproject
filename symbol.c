#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "symbol.h"


/*get two chars and integer number and enter them into new row,
if first char not exist in symbol table.
else return error to output  */
row_symbol* create_symbol_row(char *symbol, char *property, int* val){
    row_symbol *new_row;
    new_row = (row_symbol*)malloc(sizeof(row_symbol));
    if(!new_row){
        fprintf(stderr, "Cannot allocate memory!\n");
        exit(1);
    }
    new_row->symbol = symbol;
    new_row->property = property; 
    new_row->val = *val;
    return new_row;
}

/*get pointer to symbol table and pointer to row. 
add the row into symbol table */
void add_row_to_symbol_table(symbol_table *table_of_symbol, row_symbol *row){
    /*  there is no rows in the table yet, add the first row*/
    if (table_of_symbol->rows == NULL) {
        table_of_symbol->rows = (row_symbol**)malloc(sizeof(row_symbol*));
    } else {
        int nextSize = sizeof(row_symbol*) * (table_of_symbol->size+1);
        table_of_symbol->rows = (row_symbol**)realloc(table_of_symbol->rows, nextSize);
    }
    if(table_of_symbol->rows[table_of_symbol->size] == NULL){
        table_of_symbol->rows[table_of_symbol->size] = row;
        table_of_symbol->size++;   
    }  
}

/*get pointer to symbol table .
print to output the table */
void print_symbol_table(symbol_table *table_of_symbol){
    char *symbol;
    char *property;
    int i;
    int val = 0;
    for (i = 0; i < table_of_symbol->size; i++)
    {
        symbol = table_of_symbol->rows[i]->symbol;
        property = table_of_symbol->rows[i]->property;
        val = table_of_symbol->rows[i]->val;

        fprintf(stderr, "row symbol is: symbol %s, property %s, val %d.\n", symbol, property, val);
    }
 }

/*get symbol table, poiter for two 
chech if symbol exist in symbol table, if exist return error to output.
else create new row, and add into table. */
 void on_macro(symbol_table *table_of_symbol, char ** words, int num, int *has_error){
    row_symbol * row;
    char *curr_macro = words[1];
    int  val = 0;
    if(is_exist_in_symbol_table(table_of_symbol, curr_macro) == 0){
        fprintf(stderr, "Line %d: Error! label %s already exists line!\n", num, curr_macro);
        *has_error = 1;
        return;
    }

    val = atoi(words[3]);
    row = create_symbol_row(curr_macro, "macro",&val);
    add_row_to_symbol_table(table_of_symbol, row);   
}


/*get symbol table and string.
chech if given string alrady exist in table.
if exist return 0 , else 1.
 */
int is_exist_in_symbol_table(symbol_table *table_of_symbol, char *symbol){
    row_symbol *row;
    char *row_symbol;
    int i;
    int size = table_of_symbol->size;
    for(i = 0; i < size; i++){
        row = table_of_symbol->rows[i];
        row_symbol = row->symbol;
        if(strcmp(row_symbol, symbol) == 0){
            return 0;  /*symbol exist! */
        }
    }
    return 1;
}

/*get symbol table and string.
 if string exist in table, return symbol address
 else return 0;
 */
int symbol_address(symbol_table *table_of_symbol, char *str){
    row_symbol *row;
    char *row_symbol;
    int address = 0;
    int i;
    for (i = 0; i < table_of_symbol->size; i++)
    {
        row = table_of_symbol->rows[i];
        row_symbol = row->symbol;
        if(strcmp(row_symbol, str) == 0){
            address = row->val;
            return address;
        }
    }
    return 0;
}

/* get symbol table and poinrt to integer
update value of data symbols.*/
void change_val(symbol_table *table_of_symbol,int *IC){
    char *curr_prop;
    int i;
    for (i = 0; i < (table_of_symbol->size); i++)
    {
        curr_prop = table_of_symbol->rows[i]->property;
        if(strcmp(curr_prop, "data") == 0){
            table_of_symbol->rows[i]->val = table_of_symbol->rows[i]->val + (*IC);
        }
    }
}

/*get symbol table and string.
 check if property of symbol is extern/
 return 1 if extern symbol exist, else 0.*/
int is_extern(symbol_table *table_of_symbol, char *string){
    int i;
    for (i = 0; i < (table_of_symbol->size); i++)
    {
        if(strcmp(table_of_symbol->rows[i]->property, string) == 0){
            return 1;
        }
    }
    return 0;
}

/*free all data in symbol table. */
void free_symbol_table(symbol_table *table){
    int i;
    for ( i = 0; i < table->size; i++)
    {
        free(table->rows[i]);
    }
    free(table);
}


