#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "analyzer.h"
#include "helpers.h"
#include "symbol.h"
#include "operations.h"
#include "tokens.h"

void parse(char* path);

/*
    Entry point of the program
*/
int main(int argc, char* argv[]){
    int i;
    if (argc < 2) {
        fprintf(stderr, "No files was provided");
        return 1;
    }

    for (i = 1; i < argc; i++){
        parse(argv[i]);
    }

    return 0;
}

/* parse the given file as assembler file */
void parse(char* path) {
    int d = 0;
    int a = 100;
    int b = 0;
    int c = 100;
    int ex = 0;
    int ent = 0; 
    int *size;
    int temp = 0;  
    int *has_error;
    int *has_extern;
    int *has_entry;
    int *IC;
    int *DC;
    int *da;
    int initSize = 0;
    int i; 
    table_of_operations *operations_table;
    symbol_table *table_of_symbols;
    FILE *f;
    char **file;
    FILE *output;
    FILE *extern_output;
    FILE *entry_output;
    tokenized_line* l;
    operations_table = new_operations_table();
    table_of_symbols = new_symbol_table();

    IC = &a;
    DC = &b;
    da = &c;
    
    has_error = &temp;
    has_entry = &ent;
    has_extern = &ex;
    f = open_file(path, "as", "r");
    if ( f == NULL ){
        fprintf(stderr, "Failed to open file %s.as\n", path);
        exit(1);
    }
    
    
    size = &initSize;
    file = read_file(f, 81, size);


    
    for (i = 0; i < *size; i++){
        l = split(file[i]);
        if( l != NULL && l->size > 0){
            analyze_line(operations_table, table_of_symbols, l, IC, DC, file[i], i+1, has_error, has_extern, has_entry);
            free(l->tokens);
             
    
            free(l);
             
        }else{
            free(l);
             
            continue;
        }
    }
    (*da) = (*IC) ;
    update_values(table_of_symbols, IC); 

    change_decimal_address_val(operations_table, da);

    d = 100;
    
    IC = &d; 

    if (*has_extern == 1) {
        extern_output = open_file(path, "ext", "w");
    } else {
        extern_output = NULL;
    }

    if (*has_entry == 1) {
        entry_output = open_file(path, "ent", "w");
    } else {
        entry_output = NULL;
    }

    for (i = 0; i < *size; i++){
        l = split(file[i]);
        if(l->size != 0){
            analyze_line_secondly(operations_table, table_of_symbols, l, IC, i+1, has_error, extern_output, entry_output);
            free(l->tokens);
             
            free(l);
             
        }else{
            free(l);
             
            continue;
        }
    }
    (*IC)++;
    print_table_of_operations(operations_table);

    if(*has_error != 1){
        output = open_file(path, "ob", "w");
        build_object_file(output, operations_table, (*IC -100) , *DC);
    } else {
        output = NULL;
    }

    free_symbol_table(table_of_symbols);
    free_operations_table(operations_table);
    for ( i = 0; i < *size; i++)
    {
        free(file[i]);
         
    }
    free(file);
     

    fclose(f);
    if (extern_output != NULL) {
        fclose(extern_output);
    }

    if (entry_output != NULL){
        fclose(entry_output);
    }
    if (output != NULL) {
        fclose(output);
    }
}