#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "helpers.h"
#include "symbol.h"
#include "operations.h"
#include "tokens.h"

#define MAX_COMA 80
#define MACRO ".define"
#define COLON ":"
#define DOT "."
#define HASH "#"
#define MAX_CHAR 31
#define MAX_CHAR 31
#define MAX_LINE_SIZE 256
#define MAX_LINE_SIZE_ALLOWED 81

int alloc_total = 0;
int free_total = 0;

char *opcode[16]= {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red",
 "prn","jsr", "rts", "stop"};

char *opcode_to_binary[16] = { "0000", "0001", "0010", "0011", "0100", "0101", "0110", "0111", "1000", "1001","1010","1011", "1100", "1101", "1110", "1111"}; 

char *guidance[4] = {".data", ".string", ".entry", ".extern"};

char *registers[8] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

char *register_to_binary[8] = { "000", "001", "010", "011", "100", "101", "110", "111"};

char *op1_to_hash[4] = {"mov", "cmp", "add", "sub"};
char *op2_to_hash[2] = {"cmp", "prn"};

char *op1_to_registers[4] = {"mov", "cmp", "add", "sub"};
char *op2_to_registers[14] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn","jsr"};

char *op1_to_label[5] = {"mov", "cmp", "add", "sub", "lea"};
char *op2_to_label[14] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red", "prn","jsr"};

char *op1_to_label_array[5] = {"mov", "cmp", "add", "sub", "lea"};
char *op2_to_label_array[11] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "red", "prn"};

/****************** declerations ***********************************************************************/
void analyze_line(table_of_operations *table ,symbol_table *table_of_symbol, tokenized_line *t, int *IC, int *DC, char *soruce, int num, int *has_error);
int is_opcode(char *code);
int is_directive(char *directive_cmd);
int legal_name(char *string);

int is_op1_to_hash(char *string);
int is_op2_to_hash(char *string);
int is_op1_to_registers(char *string);
int is_op2_to_registers(char *string);
int is_op1_to_label(char *string);
int is_op2_to_label(char *string);
int is_op1_to_label_array(char *string);
int is_op2_to_label_array(char *string);
int check_comma(tokenized_line *t);


void act_on_istruction_line(int *IC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol, char *source, int num, int *has_error);
void act_on_directive_line(int *DC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol, int num, int *has_error);
void analyze_line_secondly(table_of_operations *table, symbol_table *table_of_symbol, tokenized_line *t, int *IC, int num, int *has_error2);
void act_on_istruction_line2(int *IC, tokenized_line *t, symbol_table *table_of_symbol, table_of_operations *table, int num, int *has_error2);
int is_register(char *string);
char* trans_opcode_to_binary(char *code);
char* trans_register_to_binary(char *code);
binary_code_table* create_binary_table_based_on_row_istru(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol, int num, int *has_error);
binary_code_table* create_binary_table_based_on_row_direc_data(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol, int i_num, int num, int *has_error);
binary_code_table* create_binary_table_based_on_row_direc_string(binary_code * code, char *str, symbol_table *table_of_symbol);
char* int_to_binary(int buffer_size, int value);
char* exclude_label(char *string);
char* exclude_index_from_label(char *string);
void parse(char* path);
void build_object_file(FILE *output, table_of_operations* operations_table,int IC ,int DC);
/****************** functions ***********************************************************************/


/* Get line from input file.
analyze the type of line */

void analyze_line(table_of_operations *table ,symbol_table *table_of_symbol, tokenized_line *t, int *IC, int *DC, char *source, int num, int *has_error){
    char *token = remove_end_of_line(t->tokens[0]);
    char *token2 = remove_end_of_line(t->tokens[1]); 
    char *macro = MACRO;
   
    
    if(token[0] == ';'){ /* this line is a note line */
        return;
    }

    else if(strcmp(token,macro) == 0 ||  strcmp(token2,macro) == 0){ /* this line is a macro line */
        if(strcmp(token2,macro) == 0){      /*TODO - move to on macro? */
            fprintf(stderr, "Line %d: Error! macro cannot have a label.\n", num);
            *has_error = 1;
            return;
        }
        else if(legal_name(token2) == 0){
            fprintf(stderr, "Line %d: Error! macro %s is iligal.\n", num, token2);
            *has_error = 1;
            return;
        }
        on_macro(table_of_symbol,t->tokens, num, has_error);
        return;
    }
    
    else if((is_opcode(token)) || (is_opcode(token2))){ /* this is istruction line */
        act_on_istruction_line(IC, t, table, table_of_symbol, source, num, has_error);
    }
     
    else if(str_begin_with(token, DOT) || str_begin_with(token2,DOT)){ /*this is direction line */
        if(is_directive(token) || is_directive(token2)){
            act_on_directive_line(DC, t, table, table_of_symbol, num, has_error);
        }
    }else{ 
        fprintf(stderr, "Line %d: Error! this line is illegal.\n", num);
        *has_error = 1;
        return;
    }
}

/* Get line from input file.
looking for instruction lines, and directive lines that are entrys.*/
void analyze_line_secondly(table_of_operations *table, symbol_table *table_of_symbol, tokenized_line *t, int *IC, int num, int *has_error2){
    int flag_entry = 0;
    int address = 0;
    char *token = remove_end_of_line(t->tokens[0]);
    char *token2 = remove_end_of_line(t->tokens[1]);  
    
    if(check_comma(t) == 0){ 
        *has_error2 = 1;
        return;
    }

    if(strcmp(token,MACRO) == 0){ /* already care in first trans */
        return;
    }

    if((is_opcode(token)) || (is_opcode(token2))){ /* instruction line */
        act_on_istruction_line2(IC, t, table_of_symbol, table, num, has_error2);
    }
    if(str_begin_with(token, DOT) || str_begin_with(token2,DOT)){
        if(strcmp(token, ".entry") == 0){/*  TODO chack if label exist*/
            address = symbol_address(table_of_symbol, token2);
            flag_entry = 1;
        }else{ 
            address = symbol_address(table_of_symbol, t->tokens[2]);
            flag_entry = 1;
            printf("kkfk%d", flag_entry);
        }
        printf("kkfk%d", address);
        /* need to create entry file with lable and address */
    }
}

/* update address for symbols that was not defined in first trans.
If that symbole are not exist in symbol table, than print error for output */
void act_on_istruction_line2(int *IC, tokenized_line *t, symbol_table *table_of_symbol, table_of_operations *table, int num, int *has_error2){
    int i;
    int num_of_operand = 0;
    int num_of_registers = 0;
    char *str;
    char *into_are;
    char *address_binary;
    int address_lebel = 0;
    char *curr;
    for (i = 0; i <= t->size; i++){
        str = trim_comma(t->tokens[i]);
        if(i == t->size){
            str = remove_end_of_line(t->tokens[i]);
        }
        
        if(strcmp(str, ",") == 0){
            continue;
        }

        if(str_ends_with(str, COLON)){
            continue;
        }
        else if(is_opcode(str)){
            if(*IC != 100){
                (*IC)++;
            }
            continue;
        }
        else if(is_register(str)){
            num_of_operand++;
            num_of_registers++;
            if(num_of_registers < 2){
                (*IC)++;
            }
            continue;
        }
        else if(str_begin_with(str, HASH)){
            num_of_registers++;
            (*IC)++;
        }
        else{ /* it is a label */
            
            if(is_label_array(str)){
                (*IC)++;
                curr = exclude_label(str);
                if(is_exist_in_symbol_table(table_of_symbol, curr) == 0){
                    if(is_extern(table_of_symbol, curr)){
                        into_are = "01";
                        address_binary = int_to_binary(12, 0);
                    }else{
                        into_are = "10";
                        address_lebel = symbol_address(table_of_symbol, curr);
                        address_binary = int_to_binary(12, address_lebel);
                    }
                    add_address_val(table, IC, address_binary, into_are);
                }else{
                    fprintf(stderr, "Line %d: Error! label %s is not defined!\n", num, str);
                    *has_error2 = 1;
                    return;
                }
                (*IC)++;
            }else{
                (*IC)++;
               
                if(is_exist_in_symbol_table(table_of_symbol, str) == 0){
                    if(is_extern(table_of_symbol, str)){
                        into_are = "01";
                        address_binary = int_to_binary(12, 0);
                    }else{
                        into_are = "10";
                        address_lebel = symbol_address(table_of_symbol, str);
                        address_binary = int_to_binary(12, address_lebel);
                    }
                    add_address_val(table, IC, address_binary, into_are);

                    
                }else{
                    fprintf(stderr, "Line %d: Error! label  %s is not defined!\n", num, str);
                    *has_error2 = 1;
                    return;
                }
            }
        }
    } 
}

/*decipher the content of instructin line.
If this is a legal line, the function create the addresses for this line end insert them to table.
Else print error for output  */
void act_on_istruction_line(int *IC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol, char *source, int num, int *has_error){
    int i;
    int num_of_da = 0;
    int num_of_operand = 0; int num_of_registers = 0;
    int curr_IC = *IC; 
    char *curr_opcode; 
    char *opcode_to_binary, *operand_1 = "", *operand_2 = "", *are = "00";
    char *str;
    row_symbol *new_symbol;
    binary_code *binary_code;
    binary_code_table *b_table;
    decimal_table *d_table;

    if(check_comma(t) == 0){
        fprintf(stderr, "Line %d: illegal commas!\n", num);
        *has_error = 1;
        return;
    }

    for(i = 0; i <= t->size ; i++){
 
        str = trim_comma(t->tokens[i]); 
        if(i == t->size){
            str = remove_end_of_line(t->tokens[i]);
        }
        if(strcmp(str, ",") == 0){
            continue;
        }
        if(num_of_operand > 2 || num_of_registers > 2){ /* in case that this line is illegal, more than 2 operands*/
            fprintf(stderr, "Line %d: Error! too many operands in this line!\n", num);
            *has_error = 1;
            return;
        }

        if(str_ends_with(str, COLON)){ 
            if(legal_name(str) == 0){
                fprintf(stderr, "Line %d: Error! label %s is illegal!\n", num, str);
                *has_error = 1;
                return;
            }
            if(is_exist_in_symbol_table(table_of_symbol, str) == 0){
                fprintf(stderr, "Line %d: Error! label %s already exists line!\n", num, str);
                *has_error = 1;
                return;
            }
            
            new_symbol = create_symbol_row(remove_colon(str), "code", &curr_IC);
            add_row_to_symbol_table(table_of_symbol ,new_symbol);
        }
        
        else if(is_opcode(str)) {
            (*IC)++;
            curr_opcode = str;
            if(strcmp(curr_opcode, "stop") == 0 || strcmp(curr_opcode, "rts") == 0){
                opcode_to_binary = trans_opcode_to_binary(curr_opcode);
                operand_1 = "00";
                operand_2 = "00";
                break;
            }
            opcode_to_binary = trans_opcode_to_binary(str);
        }

        else if(is_register(str)){ 
            if(num_of_operand == 0){
                if(is_op1_to_registers(curr_opcode)){
                    operand_1 = "11";
                }
                else if((num_of_operand == 0) && (is_op2_to_registers(curr_opcode)) ){
                    operand_1 = "00";
                    operand_2 = "11";
                }else{
                    fprintf(stderr, "Line %d: Error! illegal operand %s for this opcode.\n", num, str);
                    *has_error = 1;
                    return;
                }        
            }else{
                if(is_op2_to_registers(curr_opcode)){
                     operand_2 = "11";
                }else{
                    fprintf(stderr, "Line %d: Error! illegal operand %s for this opcode.\n", num, str);
                    *has_error = 1;
                    return;
                }        
            }
            num_of_operand++;
            num_of_registers++;
            if(num_of_registers < 2){
                (*IC)++;
            }   
        }

        else if(str_begin_with(str, HASH)){  /*operand that hash attached to number */
            if(num_of_operand == 0){
                if(is_op1_to_hash(curr_opcode)){
                    operand_1 = "00";
                }
                else if((num_of_operand == 0) && (is_op2_to_hash(curr_opcode)) ){
                    operand_1 = "00";
                    operand_2 = "00";
                }else{
                   fprintf(stderr, "Line %d: Error! illegal operand %s for this opcode.\n", num, str);
                    *has_error = 1;
                    return;
                }
            } 
            else{
                if(is_op2_to_hash(curr_opcode)){
                    operand_2 = "00";
                }else{
                    fprintf(stderr, "Line %d: Error! illegal operand %s for this opcode.\n", num, str);
                    *has_error = 1;
                    return;
                }
            }             
            num_of_operand++;
            (*IC)++; 
        }

        else { /*label defined or undefined */
            if(is_label_array(str)){
                (*IC) = (*IC) + 2;
                if(num_of_operand == 0){
                    if(is_op1_to_label_array(curr_opcode)){
                        operand_1 = "10";
                    }
                    else if((num_of_operand == 0) && (is_op2_to_label_array(curr_opcode)) ){
                        operand_1 = "00";
                        operand_2 = "10";
                    }else{
                        fprintf(stderr, "Line %d: Error! illegal operand %s for this opcode.\n", num, str);
                        *has_error = 1;
                        return;
                    } 
                }else{
                    if(is_op2_to_label_array(curr_opcode)){
                        operand_2 = "10";
                    }else{
                     fprintf(stderr, "Line %d: Error! illegal operand %s for this opcode.\n", num, str);
                     *has_error = 1;
                        return;
                    }
                }
                num_of_operand++;
            }else{ /* just a labels */ 
                 (*IC)++;
                 if(num_of_operand == 0){
                    if(is_op1_to_label(curr_opcode)){
                        operand_1 = "01";
                    }
                    else if((num_of_operand == 0) && (is_op2_to_label(curr_opcode)) ){
                        operand_1 = "00";
                        operand_2 = "01";
                    }else{
                       fprintf(stderr, "Line %d: Error! illegal operand %s for this opcode.\n", num, str);
                        *has_error = 1;
                        return;
                    }
                 }else{
                    if(is_op2_to_label(curr_opcode)){
                        operand_2 = "01";
                    }else{
                       fprintf(stderr, "Line %d: Error! illegal operand %s for this opcode.\n", num, str);
                        *has_error = 1;
                        return;
                    }
                }
                num_of_operand++;
            }
        }
    } /*end of for */
    binary_code = create_binary_code(opcode_to_binary, operand_1, operand_2, are, NULL);
    b_table = create_binary_table_based_on_row_istru(binary_code, t, table_of_symbol, num, has_error);
    
    /*initiate table of decimal addresses */
    d_table = (decimal_table*)malloc(sizeof(decimal_table));
    alloc_total++;
    alloc_total++;
    d_table->size = 0;
    d_table->decimal_address = NULL;

    num_of_da = (*IC) - curr_IC;  /* num of rows*/ 

    for (i = 0; i < num_of_da; i++)
    {
        add_row_to_decimal_table(d_table, curr_IC++); 
    }
    add_row_to_table_of_operations(table, create_operation_row(d_table, source, b_table, create_explanation_table()));
}

/*decipher the content of directive line.
If this is a legal line, the function create the addresses for this line end insert them to table.
Else print error for output  */
void act_on_directive_line(int *DC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol, int num, int *has_error){
    int i;
    int j;
    int k;
    int curr_DC; 
    int temp = -1;
    int *curr_da;
    int flag_data = 0; 
    int flag_string = 0; 
    int flag_extern = 0;
    int num_quot = 0; int flag_done = 0;
    char* word;
    char *dest;
    row_symbol *new_symbol;
    binary_code *code_binary;
    binary_code_table *b_table;
    decimal_table *d_table;
    operation_row *data_line;
    curr_da = &temp;
    

    for (i = 0; i <= t->size; i++){
        if(str_ends_with(t->tokens[i], COLON)){ 
            if(legal_name(t->tokens[i]) == 0){
                fprintf(stderr, "Line %d: Error! label %s is illegal!\n", num, t->tokens[i]);
                *has_error = 1;
                return;
            }
            if(is_exist_in_symbol_table(table_of_symbol, t->tokens[i]) == 0){
                fprintf(stderr, "Line %d: Error! label %s already exists line!\n", num, t->tokens[i]);
                *has_error = 1;
                return;
            }
            curr_DC = *DC;
            new_symbol = create_symbol_row(remove_colon(t->tokens[i]), "data", &curr_DC);
            add_row_to_symbol_table(table_of_symbol ,new_symbol);
        }
        else if(is_directive(t->tokens[i])){
            if(strcmp(t->tokens[i], ".data") == 0){
                if(check_comma(t) == 0){
                    fprintf(stderr, "Line %d: illegal commas!\n", num);
                    *has_error = 1;
                    return;
                }
                flag_data = 1;
                continue;
            }
            else if(strcmp(t->tokens[i], ".string") == 0){
                flag_string = 1;
                continue;
            }
            else if(strcmp(t->tokens[i], ".extern") == 0){
                flag_extern = 1;
                continue;
            }else{ /*entry */
                continue;
            }
        }
        else if(flag_data == 1){  /*it is data directive */
            code_binary = create_binary_code(NULL, NULL, NULL, NULL, t->tokens[i]);
            b_table = create_binary_table_based_on_row_direc_data(code_binary, t, table_of_symbol, i, num, has_error);
            /*initiate table of decimal addresses */
            d_table = (decimal_table*)malloc(sizeof(decimal_table));
            alloc_total++;
            d_table->size = 0;
            d_table->decimal_address = NULL;

            for (k = i; k <= t->size; k++)
            {
                if(strcmp(t->tokens[k], "\n") == 0){
                        continue;
                }
                if(strcmp(t->tokens[k], ",") == 0){
                   continue;
                }
                
                add_row_to_decimal_table(d_table, *curr_da);
                (*DC)++;    
            }
            data_line = create_operation_row(d_table, "", b_table, create_explanation_table());
            add_row_to_table_of_operations(table, data_line);
            break;
        }
        else if(flag_string == 1){ 
            word = remove_end_of_line(t->tokens[i]);
            for (j = 0; j <= strlen(t->tokens[i]); j++)
            {
                dest = malloc(sizeof(char));
                alloc_total++;
                strncpy(dest, t->tokens[i]+j, 1); 
                if(strcmp(dest, "\"") == 0 ){
                    num_quot++;
                    continue;
                }
                if(strcmp(dest, "") == 0 && num_quot <2){
                    code_binary = create_binary_code(NULL, NULL, NULL, NULL, dest);
                    b_table = create_binary_table_based_on_row_direc_string(code_binary, dest, table_of_symbol);
                }
                else if(j  == strlen(word)){ 
                    code_binary = create_binary_code(NULL, NULL, NULL, NULL, "0");
                    b_table = create_binary_table_based_on_row_direc_string(code_binary, "0", table_of_symbol);
                }else{
                    code_binary = create_binary_code(NULL, NULL, NULL, NULL, dest);
                    b_table = create_binary_table_based_on_row_direc_string(code_binary, dest, table_of_symbol);
                }
                /*initiate table of decimal addresses */
                d_table = (decimal_table*)malloc(sizeof(decimal_table));
                alloc_total++;
                d_table->size = 0;
                d_table->decimal_address = NULL;
                add_row_to_decimal_table(d_table, *curr_da);
        
                data_line = create_operation_row(d_table, "", b_table, create_explanation_table());
                add_row_to_table_of_operations(table, data_line);
                (*DC)++; 
                if(j == strlen(word) && num_quot == 2){
                    flag_done = 1;
                }
            } 
        }
        if(flag_done == 1){
            break;
        }
        else if(flag_extern == 1){ 
                if(is_exist_in_symbol_table(table_of_symbol, t->tokens[i])){
                    new_symbol = create_symbol_row(t->tokens[i], "extern", 0);
                    add_row_to_symbol_table(table_of_symbol ,new_symbol);   
                }else{
                    fprintf(stderr, "Line %d: Error! label %s already exists line!\n", num, t->tokens[i]);
                    *has_error = 1;
                    return;
                }                 
        }
        else{ 
            continue;
        } 
    }/* end of for */
}

/*get insruction line and create the addresses for opreands and adds them into table binary machine code.
Function returns pointer to binary code for that line*/
binary_code_table* create_binary_table_based_on_row_istru(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol, int num, int *has_error){
    int i;
    int should_check = 0;  int num_operand = 0; int num_register = 0;
    char temp;
    char* bin;
    char *str;
    char *curr_str;
    char *address;
    char *curr ;
    char *index;
    binary_code *code_binary ;
    binary_code *code_binary1; binary_code *code_binary2;
    binary_code_table *b_table;
    /*initiate table of binary code */
    b_table = (binary_code_table*)malloc(sizeof(binary_code_table));
    alloc_total++;
    b_table->size = 0;
    /*// b_table->binary_code = NULL;*/
    add_row_to_binary_code_table(b_table, code);
    
    for (i = 0; i <= t->size; i++){
        
        str = trim_comma(t->tokens[i]);
        if(i == t->size){
            str = remove_end_of_line(str);
        }
        if(num_operand >= 2){
            continue;
        }
        if(strcmp(str, ",") == 0){
            continue;
        }
        if(str_ends_with(str, COLON)){ /*it is a label*/
            continue;
        }
        else if(is_opcode(str)){
            continue;
        }
        else if(str_begin_with(str, HASH)){ /* immediate address */
            curr_str = remove_hash(str);
            if(curr_str[0] == '-'){ /* if number attached to hash is started with subtraction sign */
                curr_str = remove_sub(str);
            }
            if(curr_str[0] == '+'){ /*if number attached to hash is started with add sign */
                curr_str = remove_plus(str);
            }
            temp = *curr_str;
            if(is_exist_in_symbol_table(table_of_symbol, curr_str) == 0){ 
                address = int_to_binary(12, symbol_address(table_of_symbol, curr_str));
                code_binary = create_binary_code(NULL, NULL, NULL, "00",address);
            }else if(isdigit(temp)){
                bin = int_to_binary(12, atoi(str+1));
                code_binary = create_binary_code(NULL, NULL, NULL, "00", bin);
            }
            else{
                fprintf(stderr, "Line %d: Erorr! no such symbol %s", num ,curr_str);
                *has_error = 1;
                break;
            }
            add_row_to_binary_code_table(b_table, code_binary);    
            num_operand++;   
        }
        else if(is_register(str)){ /* direct register address */
            if(num_register == 0){
                code_binary = create_binary_code(NULL, trans_register_to_binary(str), "000", "00", NULL);
                num_register++;
                num_operand++;
                should_check = 1;
                add_row_to_binary_code_table(b_table, code_binary);   
            }
            else if(num_register == 1 && num_operand == 1){ 
                num_register++;
                num_operand++;
                if (should_check == 1){
                   if(strcmp(t->tokens[i-1], ",") == 0){
                        b_table->binary_code[i-2]->operand_destination = trans_register_to_binary(str); 
                   }else{
                       b_table->binary_code[i-1]->operand_destination = trans_register_to_binary(str);
                   }
                }
            }
            else{  /*sec operand is register */
                code_binary = create_binary_code(NULL, "000", trans_register_to_binary(str),"00", NULL);
                num_operand++;
                num_register++;
                add_row_to_binary_code_table(b_table, code_binary);   
            }
        }
        else{  /* permanent index address */
            if(is_label_array(str)){ 
                num_operand++;   
                curr = exclude_label(str);
                if(legal_name(curr) == 0){
                    fprintf(stderr, "Line %d: Error! label %s is illegal!\n", num, str);
                    *has_error = 1;
                    break;
                }
                code_binary1 = create_binary_code(NULL, NULL, NULL, NULL, NULL);
                index = exclude_index_from_label(str);
                temp = *index;
                if(isdigit(temp)){
                    bin = int_to_binary(12, atoi(index));
                    code_binary2 = create_binary_code(NULL, NULL, NULL, "00", bin);
                }else{ /*is a symbol */
                    if(is_exist_in_symbol_table(table_of_symbol, index) == 0){
                        address = int_to_binary(12, symbol_address(table_of_symbol, index));
                        code_binary2 = create_binary_code(NULL, NULL, NULL, "00", address);
                    }else{
                        fprintf(stderr, "Line %d: Erorr! no such symbol %s.\n", num, index);
                        *has_error = 1;
                        break;
                    }
                }  
                add_row_to_binary_code_table(b_table, code_binary1);
                add_row_to_binary_code_table(b_table, code_binary2);
            }
            else{ /* direct address */
                num_operand++;
                if(legal_name(str) == 0){
                    fprintf(stderr, "Line %d: Error! label %s is illegal!\n", num, str);
                    *has_error = 1;
                    break;
                }
                code_binary = create_binary_code(NULL, NULL, NULL, NULL, NULL);
                add_row_to_binary_code_table(b_table, code_binary);
            }
        }
    }/* end of for */
    return b_table;
}

/*get directive data line and create the addresses for values and adds them into table binary machine code.
Function returns pointer to binary code for that line*/
binary_code_table* create_binary_table_based_on_row_direc_data(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol, int i_num, int num, int *has_error){
    int i;
    
    binary_code_table *b_table ;
    binary_code *code_binary;
    char *str;
    char *bin; 
    char *curr_str;
    char temp;
    /*initiate table of binary code */
    b_table = (binary_code_table*)malloc(sizeof(binary_code_table));
    alloc_total++;
    b_table->size = 0;
    /*// b_table->binary_code = NULL;
    // add_row_to_binary_code_table(b_table, code); */
    
    /* int  is_negativ = 0;*/

    for (i = 0; i <= t->size; i++){
        str = trim_comma(t->tokens[i]);
        if(i == t->size){
            if(strcmp(str, "\n") == 0){
                continue;
            }else{
                str = remove_end_of_line(str);
            }     
        }
        
        if(strcmp(str, ",") == 0){
            continue;
        }     
        if(str_ends_with(str, COLON)){ /* it is a label*/
            continue;
        }
        if(is_directive(str)){
            continue;
        }
        if(str[0] == '-'){ /* if number is started with subtraction sign */
            curr_str = remove_sub(str);
        }
        else if(str[0] == '+'){ /* if number is started with subtraction sign */
            curr_str = remove_plus(str);
        }else{
            curr_str = str;
        }
        temp = *curr_str;
            
        if(isdigit(temp)){
            bin = int_to_binary(14, atoi(str));
        }else if(is_exist_in_symbol_table(table_of_symbol, str) == 0){ 
            bin = int_to_binary(14, symbol_address(table_of_symbol, str));
        }else{
            fprintf(stderr, "Line %d: Error! label %s is not exists.\n", num, str);
            *has_error = 1;
            break;
        }
            if(i == i_num ){
            code->address = bin;
            add_row_to_binary_code_table(b_table, code);
            continue;
        }
        code_binary = create_binary_code(NULL, NULL, NULL, NULL, bin);
        add_row_to_binary_code_table(b_table, code_binary);
    } /*end for  */  
    return b_table;
}


/*get char from directive string line.
The function create address for this char and add the address into table binary machine code.
Function returns pointer to binary code for that line*/
binary_code_table* create_binary_table_based_on_row_direc_string(binary_code * code, char *str, symbol_table *table_of_symbol){
    int i;
    binary_code_table *b_table; 
    binary_code *code_binary;
     char *bin;
    /* initiate table of binary code */
    b_table = (binary_code_table*)malloc(sizeof(binary_code_table));
    alloc_total++;
    b_table->size = 0;
   
    if(strcmp(str, "") == 0){ /* in case that we have empty char */
        bin = int_to_binary(14, 32);
    }
    for (i = 0; i < strlen(str); i++)
    {
        if(strcmp(str, "\"") == 0){
            continue;
        }
        if(isalpha(str[0])){
            bin = int_to_binary(14, (int)str[0]);
        }
        if(strcmp(str, "0") == 0){ /* in case that tise is the end of string-> '\0' */
            bin = int_to_binary(14, 0);
        }
    }        
    
    code_binary = create_binary_code(NULL, NULL, NULL, NULL, bin);
    add_row_to_binary_code_table(b_table, code_binary);
    return b_table;
}

/* get poiter to string and check if that string are equal to char in op1_to _hash array.
Function returns 1 if  strings are equals, else return 0. */
int is_op1_to_hash(char *string){
    int i;
    for (i = 0; i < 4; i++)
    {
        if(strcmp(op1_to_hash[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

/* get poiter to string and check if that string are equal to char in op2_to _hash array.
Function returns 1 if  strings are equals, else return 0. */
int is_op2_to_hash(char *string){
    int i;
    for (i = 0; i < 2; i++)
    {
        if(strcmp(op2_to_hash[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

/* get poiter to string and check if that string are equal to char in op1_to_registers array.
Function returns 1 if  strings are equals, else return 0. */
int is_op1_to_registers(char *string){
    int i;
    for (i = 0; i < 4; i++)
    {
        if(strcmp(op1_to_registers[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

/* get poiter to string and check if that string are equal to char in op2_to_registers array.
Function returns 1 if  strings are equals, else return 0. */
int is_op2_to_registers(char *string){
    int i;
    for (i = 0; i < 14; i++)
    {
        if(strcmp(op2_to_registers[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

/* get poiter to string and check if that string are equal to char in op1_to_label array.
Function returns 1 if  strings are equals, else return 0. */
int is_op1_to_label(char *string){
    int i;
    for (i = 0; i < 5; i++)
    {
        if(strcmp(op1_to_label[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

/* get poiter to string and check if that string are equal to char in op2_to_label array.
Function returns 1 if  strings are equals, else return 0. */
int is_op2_to_label(char *string){
    int i;
    for (i = 0; i < 14; i++)
    {
        if(strcmp(op2_to_label[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

/* get poiter to string and check if that string are equal to char in op1_to_label_array .
Function returns 1 if  strings are equals, else return 0. */
int is_op1_to_label_array(char *string){
    int i;
    for (i = 0; i < 5; i++)
    {
        if(strcmp(op1_to_label_array[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

/* get poiter to string and check if that string are equal to char in op2_to_label_array .
Function returns 1 if  strings are equals, else return 0. */
int is_op2_to_label_array(char *string){
    int i;
    for (i = 0; i < 11; i++)
    {
        if(strcmp(op2_to_label_array[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

/* get pointer to two dimensional array for line and check the legality of commas.
Function returns 1 if commas are legal, else return 0 */
int check_comma(tokenized_line *t){
    int i;
    char *curr_str = " ";
    char *prev = " ";
    int size_curr_str = 0;
    int size_prev = 0;
    for (i = 0; i <= t->size; i++)
    {
        prev = curr_str;
        curr_str = t->tokens[i];
        if(i == t->size){
            curr_str = remove_end_of_line(curr_str);
            if(strcmp(curr_str, "") == 0){
                if(strcmp(prev, ",") == 0 || prev[0] == ',' || prev[size_prev-1] == ','){
                    return 0;
                }
                break;
            }
        }
        size_curr_str = strlen(curr_str);
        size_prev = strlen(prev);
        if(strcmp(curr_str, ",") == 0 || curr_str[0] == ',' || curr_str[size_curr_str-1] == ','){ /*if has comma after data or opcode */
            if(is_opcode(prev) || (is_directive(prev) && strcmp(prev, ".data") == 0)){
                return 0;
            }
            else if(prev[size_prev-1] == ','){ /*if has 2 commas in a row */
                return 0;
            }
            else if(i+1 == t->size+1){  /*if has comma in the end of line */
                return 0;
            }
            
        }
        if(strcmp(prev, " ") != 0 && is_opcode(curr_str) == 0 && (is_directive(curr_str) == 0 || strcmp(curr_str, ".data") != 0)){ /*in case that no has commas to separate */
            if( is_opcode(prev) == 0 && (is_directive(prev) == 0 || strcmp(prev, ".data") != 0)){  
                if(strcmp(prev, ",") != 0 || prev[0] != ',' || prev[size_prev-1] != ','){
                    if(strcmp(curr_str, ",") != 0 && (curr_str[0] != ',')  &&  prev[size_prev-1] != ','){
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

/* get poiter to label and check if that label is legal .
Function returns 1 if  label are legal, else return 0. */
int legal_name(char *string){
    int i;
    int count = 0;
    for (i = 0; i < strlen(string); i++)
    {
       if(i == 0){ /* first char is upper or lower letter */
            if (isupper(string[0]) || islower(string[0])){
                count++;
                continue;
            }else{
                return 0;
            }
        }
        else if(isupper(string[i]) || islower(string[i]) || isdigit(string[i])){/*char are  upper or lower letteror numeric */
            count++;
           continue;
        }
        else if(i+1 == strlen(string) && string[i] == ':'){ 
            continue;
        }
        else{
            return 0;
        }
    }
    if(count > MAX_CHAR){ /* check the length of label, max 31 chars*/
        return 0;
    }
    if(is_opcode(string)){ /*if current label is equal to opcode name */
        return 0;
    }
    if(is_register(string)){  /*if current label is equal to register name */
        return 0;
    }
    if(is_directive(string)){  /*if current label is equal to guidance name */
        return 0;
    }
    return 1; 
}
 
/* get poiter to string and check if that string are equal to char in registers array .
Function returns 1 if  strings are equals, else return 0. */
int is_register(char *string){
    int i;
    for (i = 0; i < 8; i++)
    {
        if(strcmp(registers[i], string) == 0){
            return 1;
        }
    }
    return 0;
}

/*get poiter to string and check if that string are equal to char in guidance array .
Function returns 1 if  strings are equals, else return 0. */
int is_directive(char *guidance_cmd){
    int i;
    if(str_begin_with(guidance_cmd, DOT)){
        for (i = 0; i < 4; i++) /*fix the size of opcode array */
        {
            if(strcmp(guidance[i], guidance_cmd) == 0){
                return 1;
            }    
        }
    }
    else{ /*  guidence_cmd dont have char dor at the begining string*/
        for (i = 0; i < 4; i++)
        {
            if(strcmp(remove_dot(guidance[i]), guidance_cmd) == 0){
                return 1;
            } 
        }   
    }
    return 0;
}

/*get poiter to string and check if that string are equal to string in opcode array .
Function returns 1 if  strings are equals, else return 0. */
int is_opcode(char *code){
    int i;
    for (i = 0; i < 16; i++)
    {
        if(strcmp(opcode[i], code) == 0){
            return 1;
        }  
    }
    return 0;
}

/*get poiter to string and check if that string are equal to string in opcode array .
returns pointer to string if strings are equals. */
char* trans_opcode_to_binary(char *code){
    int i;
    int num_opcode;
     for (i = 0; i < 16; i++)
    {
        if(strcmp(opcode[i], code) == 0){
            num_opcode = i;
        }  
    }
    return opcode_to_binary[num_opcode];
}

/*get poiter to string and check if that string are equal to string in registers array .
returns pointer to string if strings are equals. */
char* trans_register_to_binary(char *code){ 
    int i;   
    int num_opcode;
     for (i = 0; i < 8; i++)
    {
        if(strcmp(registers[i], code) == 0){
            num_opcode = i;
        }  
    }
    return register_to_binary[num_opcode];
}

FILE* openFile(char *path, char* extention, char* mode) {
    FILE * fp;
    char* buffer;
    buffer = malloc(strlen(path) + strlen(extention) + 1);
    alloc_total++;
    sprintf(buffer, "%s.%s", path, extention);
    fp = fopen(buffer, mode);
    return fp;
}

void parse(char* path) {
    int d = 0;
    int a = 100;
    int b = 0;
    int c = 100;
    int size = 0;
    int temp = 0; int n = 0; 
    int *has_error;
    int *has_error2;
    int *IC;
    int *DC;
    int *da ;
    int i; 
    char line[81];
    table_of_operations *operations_table;
    symbol_table *table_of_symbols;
    FILE *f;
    char **file;
    char* lineCopy;
    FILE *output;
    /*initiate table of operations */
    operations_table = (table_of_operations*)malloc(sizeof(table_of_operations));
    alloc_total++;
    operations_table->size = 0;
    operations_table->rows = NULL;

    /*initiate table of symbols */
    table_of_symbols = (symbol_table*)malloc(sizeof(symbol_table*));
    alloc_total++;
    table_of_symbols->size = 0;
    table_of_symbols->rows = NULL;

    IC = &a;
    DC = &b;
    da = &c;
    
    has_error = &temp;
    has_error2 = &n;

    f = openFile(path, "as", "r");
    
    file = malloc(sizeof(char*));
    alloc_total++;
    if (file == NULL) {
        fprintf(stderr, "Failed to allocate memory, exiting\n");
    }

    while (fgets(line, sizeof(line), f)){
        lineCopy = malloc(strlen(line) * sizeof(char));
        alloc_total++;
        if (lineCopy == NULL) {
            fprintf(stderr, "Failed to allocate memory for line %s, exiting\n", line);
        }
        strcpy(lineCopy, line);
        file[size] = lineCopy;
        size++;
    }
    
    for (i = 0; i < size; i++){
        tokenized_line *l = split(file[i]);
        if( l != NULL && l->size > 0){
            analyze_line(operations_table, table_of_symbols, split(file[i]), IC, DC, file[i], i+1, has_error);
        }else{
            continue;
        }
    }

    /* in case we found an error during the first iteration exit with code 1*/
    if(*has_error == 1){
        printf("Found error/s in this file, dont need to return an output\n");
    }
    (*da) = (*IC) ;
    change_val(table_of_symbols, IC); 

    change_decimal_address_val(operations_table, da);
    

    d = 100;
    IC = &d; 



    for (i = 0; i < size; i++){
        if(split(file[i])->size != 0){
            analyze_line_secondly(operations_table, table_of_symbols, split(file[i]), IC, i+1, has_error2);
        }else{
            continue;
        }
    }
    (*IC)++;
     /*in case we found an error during the second iteration exit with code 1 */
    if(*has_error2 == 1){
         printf("Found error/s in this file, dont need to return an output\n\n");
    }



    print_table_of_operations(operations_table);
    print_symbol_table(table_of_symbols);
    output = openFile(path, "ob", "w");
    /* close file */
    if(*has_error2 != 1  || *has_error != 1){
        build_object_file(output, operations_table, (*IC -100) , *DC);
    }
    build_object_file(output, operations_table, (*IC -100) , *DC);

    free_symbol_table(table_of_symbols);
    free_operations_table(operations_table);

    for ( i = 0; i < size; i++)
    {
        free(file[i]);
        free_total++;
    }
    /*free(file);*/
}

void build_object_file(FILE *output, table_of_operations* operations_table,int IC ,int DC){
    int i;
    int j;
    int val = 0;
    int size = 0;
    int num_prev = 0;
    char *special_address;
    int found_unordred_addr = 0;

    fprintf(output, "  %d  %d\n", IC, DC);

    for (i = 0; i < operations_table->size; i++)
    {
        size = operations_table->rows[i]->decimal_table->size;
        for ( j = 0; j < size; j++)
        {        
            val = operations_table->rows[i]->decimal_table->decimal_address[j];
            if(val - num_prev == 1 || val == 100){
                special_address = convert_to_special(operations_table->rows[i]->binary_code_table->binary_code[j]);
                fprintf(output, "%d  %s\n", val, special_address);
                num_prev = val;
                operations_table->rows[i]->decimal_table->decimal_address[j] = -1;
            } else {
                found_unordred_addr = 1;
            }
        }
    }
    if (found_unordred_addr == 1){
        for (i = 0; i < operations_table->size; i++)
        {
            size = operations_table->rows[i]->decimal_table->size;
            for ( j = 0; j < size; j++)
            {      
                val = operations_table->rows[i]->decimal_table->decimal_address[j];
                if(val != -1){
                    special_address = convert_to_special(operations_table->rows[i]->binary_code_table->binary_code[j]);
                    fprintf(output, "%d  %s\n", val, special_address);
                }
            }
        }
    }
}

int main(int argc, char* argv[]){
    int i;
    if (argc < 2) {
        fprintf(stderr, "No files was provided");
        return 1;
    }

    for (i = 1; i < argc; i++){
        parse(argv[i]);
        printf("\n\n");
    }

    printf("Total time malloc was called: %d\n", alloc_total);
    printf("Total time free was called: %d\n", free_total);
    

    return 0;
}