#ifndef ANALYZER_H_
#define ANALYZER_H_

#include "operations.h"
#include "helpers.h"
#include "symbol.h"
#include "tokens.h" 


void analyze_line(table_of_operations *table ,symbol_table *table_of_symbol, tokenized_line *t, int *IC, int *DC, char *soruce, int num, int *has_error, int *has_extern, int *has_entry);
void instruction_handler(int *IC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol, char *source, int num, int *has_error);
void directive_handler(int *DC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol, int num, int *has_error);
binary_code_table* create_binary_table_based_on_instruction(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol, int num, int *has_error);
binary_code_table* create_binary_table_based_on_data(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol, int i_num, int num, int *has_error);
binary_code_table* create_binary_table_based_on_string(binary_code * code, char *str, symbol_table *table_of_symbol);

void analyze_line_secondly(table_of_operations *table, symbol_table *table_of_symbol, tokenized_line *t, int *IC, int num, int *has_error, FILE *extern_output, FILE *entry_output);
void update_missing_address(int *IC, tokenized_line *t, symbol_table *table_of_symbol, table_of_operations *table, int num, int *has_error, FILE *extern_output);

void build_object_file(FILE *output, table_of_operations* operations_table,int IC ,int DC);
int num_of_opcode_operands(char *string);
int is_directive(char *directive_cmd);
int is_legal_name(char *string);
int check_comma(tokenized_line *t);
char* translate_opcode_to_binary(char *code);
char* translate_register_to_binary(char *code);
int check_name(symbol_table *table_of_symbol, int *has_error, char *string, int num);

void print_to_stdout(int num, int *has_error, char *string);

#endif