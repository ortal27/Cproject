#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "analyzer.h"
#include "symbol.h"
#include "helpers.h"

#define COLON ":"


/*
    construct new symbol row
*/
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

/*
    add symbol row to the symbol table
*/
void add_row_to_symbol_table(symbol_table *table_of_symbol, row_symbol *row){
    int i;
    row_symbol** copy;
    int next_size = table_of_symbol->size + 1;
    /*  there is no rows in the table yet, add the first row*/
    if (table_of_symbol->rows == NULL) {
        table_of_symbol->rows = (row_symbol**)malloc(sizeof(row_symbol*));
         
    } else {

        copy = malloc(sizeof(row_symbol*) * next_size);

        for ( i = 0; i < table_of_symbol->size; i++)
        {
            copy[i] = table_of_symbol->rows[i];
        }

        free(table_of_symbol->rows);
         
        table_of_symbol->rows = copy;
    }
    table_of_symbol->rows[table_of_symbol->size] = row;
    table_of_symbol->size++;   
}

/*
    prints the symbol table to stdout
    this is helper function
*/
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




/*
    Handle macro keywords
    Add macro to symbol table if needed
*/
void macro_handler(symbol_table *table_of_symbol, char ** words, int num, int *has_error){
    row_symbol * row;
    char *curr_macro = words[1];
    int  val = 0;
    if(str_ends_with(words[0], COLON)){
         fprintf(stderr, "Line %d: Error! macro cannot have a label.\n", num);
            *has_error = 1;
            return;
    }
    if(check_name(table_of_symbol, has_error, words[1], num)){
        val = atoi(words[3]);
        row = create_symbol_row(curr_macro, "macro",&val);
        add_row_to_symbol_table(table_of_symbol, row);   
    }else{
        return;
    }
}


/*
    checks whenever the symbol exists in the table
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

/*
    returns the address from the table where the symbol is matched
    if not found returns 0
*/
int get_address(symbol_table *table_of_symbol, char *symbol){
    row_symbol *row;
    char *row_symbol;
    int address = 0;
    int i;
    for (i = 0; i < table_of_symbol->size; i++)
    {
        row = table_of_symbol->rows[i];
        row_symbol = row->symbol;
        if(strcmp(row_symbol, symbol) == 0){
            address = row->val;
            return address;
        }
    }
    return 0;
}

/*
    update values of the symbol table rows based on given IC
*/
void update_values(symbol_table *table_of_symbol,int *IC){
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


/*
    Check whenever given symbol has extern keyword
*/
int is_extern(symbol_table *table_of_symbol, char *symbol){
    int i;
    for (i = 0; i < (table_of_symbol->size); i++)
    {
        if(strcmp(table_of_symbol->rows[i]->symbol, symbol) == 0){
            if(strcmp(table_of_symbol->rows[i]->property, "extern") == 0){
              return 1;
            }
        }
    }
    return 0;
}

/*
    free all data in symbol table
*/
void free_symbol_table(symbol_table *table){
    int i;
    for ( i = 0; i < table->size ; i++)
    {
        free(table->rows[i]);
         
    }
    free(table);
     
}


/*
    constuct new symbol table
*/
symbol_table* new_symbol_table() {
    symbol_table* table;
    table = malloc(sizeof(symbol_table*));
     
    table->size = 0;
    table->rows = NULL;
    return table;
}