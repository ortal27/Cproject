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

int total_alloc = 0;
int total_free = 0;

int main(int argc, char* argv[]){
    int i;
    if (argc < 2) {
        fprintf(stderr, "No files was provided");
        return 1;
    }

    for (i = 1; i < argc; i++){
        parse(argv[i]);
    }

    printf("Total times malloc was called: %d\n", total_alloc);
    printf("Total times free was called: %d\n", total_free);

    return 0;
}

void parse(char* path) {
    int d = 0;
    int a = 100;
    int b = 0;
    int c = 100;
    int *size;
    int temp = 0; int n = 0; 
    int *has_error;
    int *has_error2;
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
    /*initiate table of operations */
    operations_table = new_operations_table();

    /*initiate table of symbols */
    table_of_symbols = new_symbol_table();

    IC = &a;
    DC = &b;
    da = &c;
    
    has_error = &temp;
    has_error2 = &n;

    f = openFile(path, "as", "r");
    
    size = &initSize;
    file = read_file(f, 81, size);

    extern_output = openFile(path, "ext", "w");
    entry_output = openFile(path, "ent", "w");
    
    for (i = 0; i < *size; i++){
        l = split(file[i]);
        if( l != NULL && l->size > 0){
            analyze_line(operations_table, table_of_symbols, l, IC, DC, file[i], i+1, has_error);
            free(l->tokens);
            total_free++;
    
            free(l);
            total_free++;
        }else{
            free(l);
            total_free++;
            continue;
        }
    }

    /* in case we found an error during the first iteration exit with code 1*/
    if(*has_error == 1){
        fprintf(stderr, "Found error/s in this file, during first run\n");
    }
    (*da) = (*IC) ;
    change_val(table_of_symbols, IC); 

    change_decimal_address_val(operations_table, da);

    d = 100;
    
    IC = &d; 

    for (i = 0; i < *size; i++){
        l = split(file[i]);
        if(l->size != 0){
            analyze_line_secondly(operations_table, table_of_symbols, l, IC, i+1, has_error2, extern_output, entry_output);
            free(l->tokens);
            total_free++;
            free(l);
            total_free++;
        }else{
            free(l);
            total_free++;
            continue;
        }
    }
    (*IC)++;
     /*in case we found an error during the second iteration exit with code 1 */
    if(*has_error2 == 1){
         fprintf(stderr, "Found error/s in this file, during second run\n\n");
    }

    print_table_of_operations(operations_table);
    output = openFile(path, "ob", "w");

    if(*has_error2 != 1 && *has_error != 1){
        build_object_file(output, operations_table, (*IC -100) , *DC);
    }

    free_symbol_table(table_of_symbols);
    free_operations_table(operations_table);
    for ( i = 0; i < *size; i++)
    {
        free(file[i]);
        total_free++;
    }
    free(file);
    total_free++;

    fclose(f);
    fclose(extern_output);
    fclose(entry_output);
    fclose(output);
}