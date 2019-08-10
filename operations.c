#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operations.h"


extern int total_alloc;
extern int total_free;


/*get pointer to decimal table, poiner to string, pointer for binary code table and anter them 
into new row.
return pointer to row*/
operation_row* create_operation_row(decimal_table *d_table, char* sourse_code, binary_code_table *b_table, explanation_table *e_table){
    operation_row *new_row;
    new_row = (operation_row*)malloc(sizeof(operation_row));
    total_alloc++;
    if(!new_row){
        printf("Cannot allocate memory!\n");
        exit(1);
    }
    new_row->decimal_table = d_table;
    new_row->sourse_code = sourse_code; 
    new_row->binary_code_table = b_table;
    return new_row;
}

/*get pointer to operation table, pointer for row.
add row into operation table */
void add_row_to_table_of_operations(table_of_operations *table_of_operations ,operation_row *row){
    int i;
    operation_row** copy;
    int nextSize = table_of_operations->size + 1;

    /*there is no rows in the table yet, add the first row */
    if (table_of_operations->rows == NULL) {
        table_of_operations->rows = malloc(sizeof(operation_row*));
        total_alloc++;
    } else {
        copy = malloc(sizeof(operation_row*) * nextSize);
        if (copy == NULL) {
            fprintf(stderr, "Failed to allocate memory");
        }
        for ( i = 0; i < table_of_operations->size; i++)
        {
            copy[i] = table_of_operations->rows[i];
        }

        free(table_of_operations->rows);
        total_free++;
        table_of_operations->rows = copy;
    }
    
    table_of_operations->rows[table_of_operations->size] = row;
    table_of_operations->size++;
}

/*get pointer to binary code table, pointer for row.
add row into binary code table  */
void add_row_to_binary_code_table(binary_code_table *binary_table, binary_code *binary_row){
    binary_table->binary_code[binary_table->size] = binary_row;
    binary_table->size++;
}

/*get 5 pointers to strings and anter them into new row.
return pointer to row.*/
binary_code* create_binary_code(char *opcode, char *operand_origin, char *operand_destination, char *ARE, char *address){
    char* copy = NULL;
    binary_code *new_binary_code;
    new_binary_code = (binary_code*)malloc(sizeof(binary_code));
    total_alloc++;
    if(!new_binary_code){
        printf("Cannot allocate memory!\n");
        exit(1);
    }
    if (address != NULL) {
        copy = malloc(sizeof(char) * (strlen(address)));
        total_alloc++;
        strcpy(copy, address);
    }
    
    new_binary_code->opcode = opcode;
    new_binary_code->operand_destination = operand_destination;
    new_binary_code->operand_origin = operand_origin;
    new_binary_code->ARE = ARE;
    new_binary_code->address = copy;
    return new_binary_code;
}

/* */
char* binary_to_string_first_word(binary_code *code){
    char *buffer = (char*)malloc(14 * sizeof(char));
    char* operand_origin = code->operand_origin;
    char* operand_dest = code->operand_destination;
    char* are = code->ARE;
    char* op_code = code->opcode;
    char* address = code->address;

    total_alloc++;


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

/*get pointer to operation table and print her out. */
void print_table_of_operations(table_of_operations *table_of_operations){
    char *special_str;
    char *code;
    int decimal_addr = 0;
    int i; int j;
    for (i = 0; i < table_of_operations->size; i++)
    {
        for (j = 0; j < table_of_operations->rows[i]->binary_code_table->size; j++)
        {   
            code = binary_to_string_first_word(table_of_operations->rows[i]->binary_code_table->binary_code[j]);
            special_str = convert_to_special(table_of_operations->rows[i]->binary_code_table->binary_code[j]);
            decimal_addr = table_of_operations->rows[i]->decimal_table->decimal_address[j];
            printf("Decimal Addr: %d  -- Binary Machine Code: %s - %s\n", decimal_addr, code, special_str);
            free(code);
            total_free++;

            free(special_str);
            total_free++;
        }
     }
}

/*get pointer to decimal_table, integer pointer.
add integer into decimal_table  */
void add_row_to_decimal_table(decimal_table *decimal_table, int decimal_address){
    int *copy;
    int nextSize = decimal_table->size+1;
    int i;
    if (decimal_table->decimal_address == NULL) {
        decimal_table->decimal_address = (int*)malloc(sizeof(int));
        total_alloc++;
    } else {
        copy = malloc(sizeof(int) * nextSize);
        total_alloc++;
        for ( i = 0; i < decimal_table->size; i++)
        {
            copy[i] = decimal_table->decimal_address[i];
        }
        free(decimal_table->decimal_address);
        total_free++;
        decimal_table->decimal_address = copy;
    }
        
    decimal_table->decimal_address[decimal_table->size] = decimal_address;
    decimal_table->size++;
}



explanation_table *create_explanation_table(){
    explanation_table *table = (explanation_table*)malloc(sizeof(explanation_table));
    total_alloc++;
    table->size  =0;
    table->explanations = NULL;
    return table;
}

/*get pointer to operation table, integer pointer, tow pointers to string.
add integer and strings into operation table. */
void add_address_val(table_of_operations *table_of_operations, int *IC, char *address_binary, char *are){
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

/*get pointer to binary table and convert the binary machine code address into special chars.
return pointer to string. */
char* convert_to_special(binary_code *code){
    char* res = malloc(sizeof(char) * 4);
    char* str = binary_to_string_first_word(code);
    int i; 
    int l;
    char* a;
    char* b;
    int A, B;
    total_alloc++;


    for (i = 0; i < 14; i += 2)
    {
        l = i/2;
        a = (char*)malloc(sizeof(char));
        total_alloc++;
        strncpy(a, str+i, 1 );
        b = (char*)malloc(sizeof(char));
        total_alloc++;
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
        total_free++;
        free(b);   
        total_free++;
    }

    free(str);
    total_free++;

    return res;
}

/*get opreation table and pointer to integer.
update decimal address of directive lines. */
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
/*clear all data in operation table */
void free_operations_table(table_of_operations *table) {
    int i,j;
    for ( i = 0; i < table->size; i++)
    {
        if (table->rows[i]->binary_code_table != NULL) {
            for ( j = 0; j < table->rows[i]->binary_code_table->size; j++)
            {   /* printf("index %d\n", j);
                if(table->rows[i]->binary_code_table->binary_code[j]->address != NULL){
                    printf("Releasing %s\n", table->rows[i]->binary_code_table->binary_code[j]->address);
                    free(table->rows[i]->binary_code_table->binary_code[j]->address);
                    total_free++;
                }*/
            }
        }

        if (table->rows[i]->decimal_table != NULL) {
            free(table->rows[i]->decimal_table);
            total_free++;
        }

        free(table->rows[i]);
        total_free++;
    }
    free(table);
    total_free++;
    
}