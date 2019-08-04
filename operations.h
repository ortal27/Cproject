#ifndef OPERATIONS_H_
#define OPERATIONS_H_

typedef struct{
    char *opcode;
    char *operand_origin;
    char *operand_destination;
    char *ARE;
    char *address;
}binary_code;


typedef struct {
    binary_code *binary_code[10];
    int size;
} binary_code_table;

typedef struct {
    int *decimal_address;
    int size;
}decimal_table;

typedef struct {
    char **explanations;
    int size;
} explanation_table; // do not use yet! (love you :-*)

typedef struct{
    decimal_table *decimal_table;
    char* sourse_code;
    explanation_table* explanation_table;
    binary_code_table *binary_code_table;
}operation_row;

typedef struct{
    operation_row **rows;
    int size ;
}table_of_operations;

void add_row_to_table_of_operations(table_of_operations *table_of_operations ,operation_row *row);
binary_code* create_binary_code(char *opcode, char *operand_origin, char *operand_destination, char *ARE,  char *address);
void print_table_of_operations(table_of_operations *table_of_operations);
char *binary_to_string_first_word(binary_code *code);
char *binary_to_string_register(binary_code *code);
char *binary_to_string_value(binary_code *code);
operation_row* create_operation_row(decimal_table *d_table, char* sourse_code, binary_code_table *b_table, explanation_table *e_table);
void add_row_to_decimal_table(decimal_table *decimal_table, int decimal_address);
void add_row_to_explanation_table(explanation_table *explanation_table, char *explanation);
void add_row_to_binary_code_table(binary_code_table *binary_table, binary_code *binary_row);

explanation_table *create_explanation_table();


#endif