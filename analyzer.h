#ifndef ANALYZER_H_
#define ANALYZER_H_

#include "operations.h"
#include "helpers.h"
#include "symbol.h"
#include "tokens.h" 


void analyze_line(table_of_operations *table ,symbol_table *table_of_symbol, tokenized_line *t, int *IC, int *DC, char *soruce, int num, int *has_error);
void act_on_istruction_line(int *IC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol, char *source, int num, int *has_error);
void act_on_directive_line(int *DC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol, int num, int *has_error);
binary_code_table* create_binary_table_based_on_row_istru(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol, int num, int *has_error);
binary_code_table* create_binary_table_based_on_row_direc_data(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol, int i_num, int num, int *has_error);
binary_code_table* create_binary_table_based_on_row_direc_string(binary_code * code, char *str, symbol_table *table_of_symbol);

void analyze_line_secondly(table_of_operations *table, symbol_table *table_of_symbol, tokenized_line *t, int *IC, int num, int *has_error2, FILE *extern_output, FILE *entry_output);

void act_on_istruction_line2(int *IC, tokenized_line *t, symbol_table *table_of_symbol, table_of_operations *table, int num, int *has_error2);

void build_object_file(FILE *output, table_of_operations* operations_table,int IC ,int DC);

/****************** declerations ***********************************************************************/

int is_opcode(char *code);
int is_register(char *string);
int is_directive(char *directive_cmd);

int is_legal_name(char *string);
int check_comma(tokenized_line *t);

int is_immediate_address_operand1(char *string);
int is_immediate_address_operand2(char *string);
int is_direct_register_address_operand1(char *string);
int is_direct_register_address_operand2(char *string);
int is_direct_address_operand1(char *string);
int is_direct_address_operand2(char *string);
int is_permanent_index_address_operand1(char *string);
int is_permanent_index_address_operand2(char *string);

char* trans_opcode_to_binary(char *code);
char* trans_register_to_binary(char *code);

int check_legally_name(symbol_table *table_of_symbol, int *has_error, char *string, int num);

/****************** functions ***********************************************************************/

#endif