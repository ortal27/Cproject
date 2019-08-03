#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "symbole.h"



row_symbol* create_symbol_row(char *symbol, char *property, int* val){
    row_symbol *new_row;
    new_row = (row_symbol*)malloc(sizeof(row_symbol));
    if(!new_row){
        printf("Cannot allocate memory!\n");
        exit(1);
    }
    new_row->symbol = symbol;
    new_row->property = property; 
    new_row->val = *val;
    return new_row;
}

void add_row_to_symbol_table(symbol_table *table_of_symbol, row_symbol *row){
    // there is no rows in the table yet, add the first row
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

void print_symbol_table(symbol_table *table_of_symbol){
     for (int i = 0; i < table_of_symbol->size; i++)
     {
         char *symbol = table_of_symbol->rows[i]->symbol;
         char *property = table_of_symbol->rows[i]->property;
         int val = table_of_symbol->rows[i]->val;

         printf("row symbol is: symbol %s, property %s, val %d.\n", symbol, property, 
         val);
     }
 }

 void on_macro(symbol_table *table_of_symbol, char ** words){
    char *curr_macro = words[1];
    if(is_exist_in_symbol_table(table_of_symbol, curr_macro) == 0){
        exit_with_message(curr_macro);
    }

    row_symbol * row;
    int  val = atoi(words[3]);
    row = create_symbol_row(curr_macro, "macro",&val);
    add_row_to_symbol_table(table_of_symbol, row);   
}

int is_exist_in_symbol_table(symbol_table *table_of_symbol, char *symbol){
    int index = 0;
    int size = table_of_symbol->size;
    for(int i = 0; i < size; i++){
        row_symbol *row = table_of_symbol->rows[i];
        char *row_symbol = row->symbol;
        if(strcmp(row_symbol, symbol) == 0){
            return 0; // symbole exist!
        }
    }
    return 1;
}
void exit_with_message(char *symbol){
    printf("Error, %s already exsist in symbol table\n", symbol);
    exit(1);
}

int symbol_address(symbol_table *table_of_symbol, char *str){
    for (int i = 0; i < table_of_symbol->size; i++)
    {
        row_symbol *row = table_of_symbol->rows[i];
        char *row_symbol = row->symbol;
        if(strcmp(row_symbol, str) == 0){
            int address = row->val;
            return address;
        }
    }
    return 0;
}
