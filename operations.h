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


typedef struct{
    decimal_table *decimal_table;
    char* sourse_code;
    binary_code_table *binary_code_table;
}operation_row;

typedef struct{
    operation_row **rows;
    int size ;
}table_of_operations;

void add_row_to_table_of_operations(table_of_operations *table_of_operations ,operation_row *row);
binary_code* create_binary_code(char *opcode, char *operand_origin, char *operand_destination, char *ARE,  char *address);
void print_table_of_operations(table_of_operations *table_of_operations);
char *binary_code_to_string(binary_code *code);
operation_row* create_operation_row(decimal_table *d_table, char* sourse_code, binary_code_table *b_table);
void add_row_to_decimal_table(decimal_table *decimal_table, int decimal_address);
void add_row_to_binary_code_table(binary_code_table *binary_table, binary_code *binary_row);
void add_address_value(table_of_operations *table_of_operations, int *IC, char *address_binary, char *are);
char* convert_to_special(binary_code *code);
void change_decimal_address_val(table_of_operations *operations_table ,int *da);
void free_operations_table(table_of_operations *table);
table_of_operations* new_operations_table();
decimal_table* new_decimal_table();

#endif