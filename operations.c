#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operations.h"

operation_row* create_operation_row(decimal_table *d_table, char* sourse_code, binary_code_table *b_table, explanation_table *e_table){
    operation_row *new_row;
    new_row = (operation_row*)malloc(sizeof(operation_row));
    if(!new_row){
        printf("Cannot allocate memory!\n");
        exit(1);
    }
    new_row->decimal_table = d_table;
    new_row->sourse_code = sourse_code; 
   // new_row->explan = explan;
    new_row->binary_code_table = b_table;
    return new_row;
}

void add_row_to_table_of_operations(table_of_operations *table_of_operations ,operation_row *row){
    // there is no rows in the table yet, add the first row
    if (table_of_operations->rows == NULL) {
        table_of_operations->rows = (operation_row**)malloc(sizeof(operation_row*));
    } else {
        int nextSize = sizeof(operation_row) * (table_of_operations->size+1);
        table_of_operations->rows = (operation_row**)realloc(table_of_operations->rows, 
       nextSize);
    }
    
    if(table_of_operations->rows[table_of_operations->size] == NULL){
        table_of_operations->rows[table_of_operations->size] = row;
        table_of_operations->size++;
    }
}


void add_row_to_binary_code_table(binary_code_table *binary_table, binary_code *binary_row){
    // there is no rows in the table yet, add the first row
    // if (binary_table->binary_code == NULL) {
    //     binary_table->binary_code = (binary_code**)malloc(sizeof(binary_code*));
    // }
    // } else {
    //     int nextSize = sizeof(binary_code*) * (binary_table->size+1);
    //     binary_table->binary_code = (binary_code**)realloc(binary_table->binary_code, nextSize);
    // }
    
    // if(binary_table->binary_code[binary_table->size] == NULL){
        binary_table->binary_code[binary_table->size] = binary_row;
        binary_table->size++;
    // }
}


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

char* binary_to_string_first_word(binary_code *code){
    char *buffer = (char*)malloc(14 * sizeof(char));
    
    char* operand_origin = code->operand_origin;
    char* operand_dest = code->operand_destination;
    char* are = code->ARE;
    char* op_code = code->opcode;

    if (code->address != NULL){
        sprintf(buffer, "%s00", code->address);    
    } else if(are == NULL){
            sprintf(buffer, "EMPTY_CODE");    
    }
    else if ( strcmp(are, "00") == 0 ) {
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

// char* binary_to_string_register(binary_code *code){
//     char *buffer = (char*)malloc(14 * sizeof(char));
//     sprintf(buffer, "000000%s%s%s", code->address1, code->address2, code->ARE);
//     return buffer; 
// }

// char* binary_to_string_value(binary_code *code){
//     char *buffer = (char*)malloc(14 * sizeof(char));
//     sprintf(buffer, "%s%s", code->address2, code->ARE);
//     return buffer; 
// }

void print_table_of_operations(table_of_operations *table_of_operations){
    printf("Table Of Operations\n");
    for (int i = 0; i < table_of_operations->size; i++)
    {
        
        char *sourse_code = table_of_operations->rows[i]->sourse_code;

        printf("ROW: %s\n", sourse_code);
        for (int j = 0; j < table_of_operations->rows[i]->binary_code_table->size; j++)
        {   
            char *code = binary_to_string_first_word(table_of_operations->rows[i]->binary_code_table->binary_code[j]);
            int decimal_addr = table_of_operations->rows[i]->decimal_table->decimal_address[j];
            // char *explanation = table_of_operations->rows[i]->explanation_table->explanations[j];
            char *explanation = "Not data yet";
            printf("Decimal Addr: %d -- Explanation: %s -- Binary Machine Code: %s\n", decimal_addr,explanation, code);            
        }

        // if(strcmp(explan, "First_word") == 0){
        //     printf("operation row is: decimal_address %d, sourse_code %s, explanation %s, binary_code_table %s.\n",
        //  decimal_address, sourse_code, explan, binary_to_string_first_word(table_of_operations->rows[i]->binary_code_table->binary_code[i]));
        // }
        // else if(strcmp(explan, "register") == 0){
        //      printf("operation row is: decimal_address %d, sourse_code %s, explanation %s, binary_code_table %s.\n",
        //  decimal_address, sourse_code, explan, binary_to_string_register(table_of_operations->rows[i]->binary_code_table->binary_code[i]));
        // }else{
        //     printf("operation row is: decimal_address %d, sourse_code %s, explanation %s, binary_code_table %s.\n",
        //  decimal_address, sourse_code, explan, binary_to_string_value(table_of_operations->rows[i]->binary_code_table->binary_code[i]));
        //}
     }
}

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