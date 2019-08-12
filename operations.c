#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operations.h"

#define NUM_BITS 14

/*
    Contruct operation row
*/
operation_row* create_operation_row(decimal_table *d_table, char* sourse_code, binary_code_table *b_table){
    operation_row *new_row;
    new_row = (operation_row*)malloc(sizeof(operation_row));
     
    if(!new_row){
        fprintf(stderr, "Failed to allocate memory!\n");
        exit(1);
    }
    new_row->decimal_table = d_table;
    new_row->sourse_code = sourse_code; 
    new_row->binary_code_table = b_table;
    return new_row;
}

/*
    Adds new row to table of operations
*/
void add_row_to_table_of_operations(table_of_operations *table_of_operations ,operation_row *row){
    int i;
    operation_row** copy;
    int next_size = table_of_operations->size + 1;

    /*there is no rows in the table yet, add the first row */
    if (table_of_operations->rows == NULL) {
        table_of_operations->rows = malloc(sizeof(operation_row*));
         
    } else {
        copy = malloc(sizeof(operation_row*) * next_size);
        if (copy == NULL) {
            fprintf(stderr, "Failed to allocate memory");
        }
        for ( i = 0; i < table_of_operations->size; i++)
        {
            copy[i] = table_of_operations->rows[i];
        }

        free(table_of_operations->rows);
         
        table_of_operations->rows = copy;
    }
    
    table_of_operations->rows[table_of_operations->size] = row;
    table_of_operations->size++;
}

/*
    Adds row to binary code table
*/
void add_row_to_binary_code_table(binary_code_table *binary_table, binary_code *binary_row){
    binary_table->binary_code[binary_table->size] = binary_row;
    binary_table->size++;
}

/*
    Contruct binary code
*/
binary_code* create_binary_code(char *opcode, char *operand_origin, char *operand_destination, char *ARE, char *address){
    binary_code *new_binary_code;
    new_binary_code = (binary_code*)malloc(sizeof(binary_code));
     
    if(!new_binary_code){
        fprintf(stderr, "Failed to allocate memory\n");
        exit(1);
    }
    
    new_binary_code->opcode = opcode;
    new_binary_code->operand_destination = operand_destination;
    new_binary_code->operand_origin = operand_origin;
    new_binary_code->ARE = ARE;
    new_binary_code->address = address;
    return new_binary_code;
}

/*
    returns the binary representation of binary code
*/
char* binary_code_to_string(binary_code *code){
    char* operand_origin = code->operand_origin;
    char* operand_dest = code->operand_destination;
    char* are = code->ARE;
    char* op_code = code->opcode;
    char* address = code->address;
    char *buffer = (char*)malloc(14 * sizeof(char));
     


    if (address != NULL){
        if (are != NULL && strcmp(are, "10") == 0){
            sprintf(buffer, "%s%s", address, are);    
        } else if (are == NULL){
            sprintf(buffer, "%s", code->address);    
        } else {
            sprintf(buffer, "%s00", code->address);    
        }
    } else if(are == NULL){
        sprintf(buffer, "EMPTY_CODE");    
    } else if ( strcmp(are, "00") == 0 ) {
        if(op_code == NULL) {
            sprintf(buffer, "000000%s%s%s",operand_origin, operand_dest, are);    
        } else {
            sprintf(buffer, "0000%s%s%s%s", op_code, operand_origin, operand_dest, are);    
        }
    } else {
        sprintf(buffer, "0000%s%s%s%s", op_code, operand_origin, operand_dest, are);    
    }
    return buffer;
}

/*
    print the table of operation to stdout
    this is helper function
*/
void print_table_of_operations(table_of_operations *table_of_operations){
    char *special_str;
    char *code;
    int decimal_addr = 0;
    int i; int j;
    for (i = 0; i < table_of_operations->size; i++)
    {
        for (j = 0; j < table_of_operations->rows[i]->binary_code_table->size; j++)
        {   
            code = binary_code_to_string(table_of_operations->rows[i]->binary_code_table->binary_code[j]);
            special_str = convert_to_special(table_of_operations->rows[i]->binary_code_table->binary_code[j]);
            decimal_addr = table_of_operations->rows[i]->decimal_table->decimal_address[j];
            printf("Decimal Addr: %d  -- Binary Machine Code: %s - %s\n", decimal_addr, code, special_str);
            free(code);     

            free(special_str);     
        }
     }
}

/*
    adds row to decimal table
*/
void add_row_to_decimal_table(decimal_table *decimal_table, int decimal_address){
    int *copy;
    int next_size = decimal_table->size+1;
    int i;
    if (decimal_table->decimal_address == NULL) {
        decimal_table->decimal_address = malloc(sizeof(int));
         
    } else {
        copy = malloc(sizeof(int) * next_size);
         
        for ( i = 0; i < decimal_table->size; i++)
        {
            copy[i] = decimal_table->decimal_address[i];
        }
        free(decimal_table->decimal_address);
         
        decimal_table->decimal_address = copy;
    }
        
    decimal_table->decimal_address[decimal_table->size] = decimal_address;
    decimal_table->size++;
}


/*
    Adds the value of the address into table of operations
    This is invoked during the second run
*/
void add_address_value(table_of_operations *table_of_operations, int *IC, char *address_binary, char *are){
    int i; int j;
    int size = 0;
    int da = 0;
    for (i = 0; i < (table_of_operations->size); i++)
    {
        size = table_of_operations->rows[i]->decimal_table->size;
        for (j = 0; j <= size; j++)
        {
            da = table_of_operations->rows[i]->decimal_table->decimal_address[j];
            if (da == *IC) {
                binary_code *b_code = table_of_operations->rows[i]->binary_code_table->binary_code[j];
                b_code->address = address_binary;
                b_code->ARE = are;
                return;
            }
        }
    }
    
}

/*
    returns the representation of a binary code as special charaters (* # % !)
*/
char* convert_to_special(binary_code *code){
    char* res = malloc(sizeof(char) * 4);
    char* str = binary_code_to_string(code);
    int i; 
    int l;
    char* a;
    char* b;
    int A, B;
     


    for (i = 0; i < NUM_BITS; i += 2)
    {
        l = i/2;
        a = (char*)malloc(sizeof(char));
         
        strncpy(a, str+i, 1 );
        b = (char*)malloc(sizeof(char));
         
        strncpy(b, str+i+1,1);
        A = atoi(a);
        B = atoi(b);
        if (A == 0 && B == 0) {
            res[l] = '*';
        } 
        else if(A == 0 && B == 1){
            res[l] = '#';
        }
        else if(A == 1 && B == 0){
            res[l] = '%';
        }
        else{  /*a = 1 and b=1 */
            res[l] = '!';
        }
        free(a);
         
        free(b);   
         
    }

    free(str);
     

    return res;
}

/*
    Update the decimal address of line that wasnt set during the first run
*/
void change_decimal_address_val(table_of_operations *operations_table, int *da){
    int i,j;
    int size = 0;
    int val = 0;
    for (i = 0; i < operations_table->size; i++)
    {
        size = operations_table->rows[i]->decimal_table->size;
        for (j = 0; j <= size; j++)
        {
            val = operations_table->rows[i]->decimal_table->decimal_address[j];
            if( val == -1){
                operations_table->rows[i]->decimal_table->decimal_address[j] = (*da);
                (*da)++;
            }
        }
    }
}

/*
    clear all data in operation table
*/
void free_operations_table(table_of_operations *table) {
    int i,j;
    for ( i = 0; i < table->size; i++)
    {
        if (table->rows[i]->binary_code_table != NULL) {
            for ( j = 0; j < table->rows[i]->binary_code_table->size; j++)
            {   
                if(table->rows[i]->binary_code_table->binary_code[j]->address != NULL){
                    free(table->rows[i]->binary_code_table->binary_code[j]->address);
                     
                }
            }
        }

        if (table->rows[i]->decimal_table != NULL) {
            free(table->rows[i]->decimal_table);
             
        }

        free(table->rows[i]);
         
    }
    free(table);
     
    
}

/*
    construct new table of operations
*/
table_of_operations* new_operations_table() {
    table_of_operations* operations_table = malloc(sizeof(table_of_operations));
     
    operations_table->size = 0;
    operations_table->rows = NULL;
    return operations_table;
}

/*
    construct new decimal talbe
*/
decimal_table* new_decimal_table() {
    decimal_table* table = malloc(sizeof(decimal_table));
     
    table->size = 0;
    table->decimal_address = NULL;
    return table;
}