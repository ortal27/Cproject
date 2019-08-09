#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operations.h"


/*get pointer to decimal table, poiner to string, pointer for binary code table and anter them 
into new row.
return pointer to row*/
operation_row* create_operation_row(decimal_table *d_table, char* sourse_code, binary_code_table *b_table, explanation_table *e_table){
    operation_row *new_row;
    new_row = (operation_row*)malloc(sizeof(operation_row));
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
    int nextSize = 0;
    /*there is no rows in the table yet, add the first row */
    if (table_of_operations->rows == NULL) {
        table_of_operations->rows = (operation_row**)malloc(sizeof(operation_row*));
    } else {
        nextSize = sizeof(operation_row) * (table_of_operations->size+1);
        table_of_operations->rows = (operation_row**)realloc(table_of_operations->rows, 
       nextSize);
    }
    
    if(table_of_operations->rows[table_of_operations->size] == NULL){
        table_of_operations->rows[table_of_operations->size] = row;
        table_of_operations->size++;
    }
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
    binary_code *new_binary_code;
    new_binary_code = (binary_code*)malloc(sizeof(binary_code));
    if(!new_binary_code){
        printf("Cannot allocate memory!\n");
        exit(1);
    }
    
    new_binary_code->opcode = opcode;
    new_binary_code->operand_destination = operand_destination;
    new_binary_code->operand_origin = operand_origin;
    new_binary_code->ARE = ARE;
    new_binary_code->address = address;
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
    char *explanation;
    int decimal_addr = 0;
    int i; int j;
    for (i = 0; i < table_of_operations->size; i++)
    {
        for (j = 0; j < table_of_operations->rows[i]->binary_code_table->size; j++)
        {   
            code = binary_to_string_first_word(table_of_operations->rows[i]->binary_code_table->binary_code[j]);
            special_str = convert_to_special(table_of_operations->rows[i]->binary_code_table->binary_code[j]);
            decimal_addr = table_of_operations->rows[i]->decimal_table->decimal_address[j];
            explanation = "No data yet";
            printf("Decimal Addr: %d -- Explanation: %s -- Binary Machine Code: %s - %s\n", decimal_addr,explanation, code, special_str);
        }
     }
}

/*get pointer to decimal_table, integer pointer.
add integer into decimal_table  */
void add_row_to_decimal_table(decimal_table *decimal_table, int decimal_address){
    if (decimal_table->decimal_address == NULL) {
        decimal_table->decimal_address = (int*)malloc(sizeof(int));
    } else {
        int nextSize = sizeof(int*) * (decimal_table->size+1);
        decimal_table->decimal_address = (int*)realloc(decimal_table->decimal_address, 
       nextSize);
    }
    if(decimal_table->decimal_address[decimal_table->size] == 0){
        decimal_table->decimal_address[decimal_table->size] = decimal_address;
        decimal_table->size++;
    }
}


void add_row_to_explanation_table(explanation_table *explanation_table, char *explanation){
       if (explanation_table->explanations == NULL) {
        explanation_table->explanations = (char**)malloc(sizeof(char*));
    } else {
        int nextSize = sizeof(char*) * (explanation_table->size+1);
        explanation_table->explanations = (char**)realloc(explanation_table->explanations, nextSize);
    }
    if(explanation_table->explanations[explanation_table->size] == 0){
        explanation_table->explanations[explanation_table->size] = explanation;
        explanation_table->size++;
    }
}

explanation_table *create_explanation_table(){
    explanation_table *table = (explanation_table*)malloc(sizeof(explanation_table));
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
    for (i = 0; i < 14; i += 2)
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
    }

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
            {
                free(table->rows[i]->binary_code_table->binary_code[j]);
            }
        }
        free(table->rows[i]);
        /* free other tables */
    }
    free(table);
    
}