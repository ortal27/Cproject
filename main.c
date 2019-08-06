#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "helpers.h"
#include "symbole.h"
#include "operations.h"
#include "tokens.h"

#define MAX_COMA 80
#define MACRO ".define"
#define COLON ":"
#define DOT "."
#define HASH "#"

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

typedef struct {
    int *decimal_address;
    binary_code *binary_code;
    char *explanation;
} non_complete_op_row;

/****************** declerations ***********************************************************************/
void analyze_line(table_of_operations *table ,symbol_table *table_of_symbol, tokenized_line *t, int *IC, int *DC, char *soruce, int *da);
int is_opcode(char *code);
int is_directive(char *directive_cmd);

int is_op1_to_hash(char *string);
int is_op2_to_hash(char *string);
int is_op1_to_registers(char *string);
int is_op2_to_registers(char *string);
int is_op1_to_label(char *string);
int is_op2_to_label(char *string);
int is_op1_to_label_array(char *string);
int is_op2_to_label_array(char *string);

void act_on_istruction_line(int *IC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol, char *source);
void act_on_directive_line(int *DC, int *da, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol);
void analyze_line_secondly(table_of_operations *table, symbol_table *table_of_symbol, tokenized_line *t, int *IC);
void act_on_istruction_line2(int *IC, tokenized_line *t, symbol_table *table_of_symbol, table_of_operations *table);
int is_register(char *string);
char* trans_opcode_to_binary(char *code);
char* trans_register_to_binary(char *code);
binary_code_table* create_binary_table_based_on_row_istru(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol);
binary_code_table* create_binary_table_based_on_row_direc_data(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol, int num);
binary_code_table* create_binary_table_based_on_row_direc_string(binary_code * code, char *str, symbol_table *table_of_symbol, char *cmd);

char* int_to_binary(int buffer_size, int value);
char* exclude_label(char *string);
char* exclude_index_from_label(char *string);
/****************** functions ***********************************************************************/

void analyze_line(table_of_operations *table ,symbol_table *table_of_symbol, tokenized_line *t, int *IC, int *DC, char *source, int *da){
    int size = t->size;
    char *token = t->tokens[0];
    char *token2 = t->tokens[1]; 
    char *macro = MACRO;
    
    if(strcmp(token, ";")== 0){ // note line
        printf("the assembler ignoring from this line.\n");
        return;
    }
    else if(is_empty_line(t->tokens)){ // something is not working!
        printf("this is ampty line.\n");
        return;
    }

    else if(strcmp(token,macro) == 0){ // it is macro define
        on_macro(table_of_symbol,t->tokens);
        return;
    }
    
    else if((is_opcode(token)) || (is_opcode(token2))){ // it is instruction line
        act_on_istruction_line(IC, t, table, table_of_symbol, source);
        (*da) =  (*IC);
    }
     
    else if(str_begin_with(token, DOT) || str_begin_with(token2,DOT)){ // it is directive line
        if(is_directive(token) || is_directive(token2)){
            act_on_directive_line(DC, da, t, table, table_of_symbol);
        }
    }else{ 
        printf("Error! this line is illegal.\n");
        return;
    }
}

void analyze_line_secondly(table_of_operations *table, symbol_table *table_of_symbol, tokenized_line *t, int *IC){
    int flag_entry = 0;
    int size = t->size;
    char *token = t->tokens[0];
    char *token2 = t->tokens[1]; 

    if((is_opcode(token)) || (is_opcode(token2))){
        act_on_istruction_line2(IC, t, table_of_symbol, table);
    }
    if(str_begin_with(token, DOT) || str_begin_with(token2,DOT)){
        if(strcmp(token, ".entry") == 0){// TODO chack if label exist
            int address = symbol_address(table_of_symbol, token2);
            flag_entry = 1;
            // need to create entry file with lable and address 
        }
    }
}

void act_on_istruction_line2(int *IC, tokenized_line *t, symbol_table *table_of_symbol, table_of_operations *table){
    int num_of_operand = 0, num_of_registers = 0;
    int flag_comma = 0; 
    for (int i = 0; i <= t->size; i++){
        char *str = trim_comma(t->tokens[i]);
        // if(strcmp(str, ",") == 0 && flag_comma == 1){
        //     printf("Error! too many commas.\n");
        // }else{
        //     flag_comma = 0;
        // }
        // if( (t->tokens[i+1] == NULL)  && flag_comma == 1){
        //     printf("Error! too many commas.\n");
        // }
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
        else{ // it is a label
            char *into_are;
            char *address_binary;
            int address = 0;
            if(is_label_array(str)){
                (*IC)++;
                char *curr = exclude_label(str);
                if(is_exist_in_symbol_table(table_of_symbol, curr) == 0){
                    if(is_extern(table_of_symbol, curr)){
                        into_are = "01";
                        address_binary = int_to_binary(12, 0);
                    }else{
                        into_are = "10";
                        address = symbol_address(table_of_symbol, curr);
                        printf("The adddress of that symbol is : %d\n", address);
                        address_binary = int_to_binary(12, address);
                         printf("The binary adddress of that symbol is : %s\n", address_binary);
                    }
                    add_address_val(table, IC, address_binary, into_are);
                }else{
                    printf("Error!, this label is not defined!\n");
                }
                (*IC)++;
            }else{
                (*IC)++;
                char *address_binary;
                int address = 0;
                if(is_exist_in_symbol_table(table_of_symbol, str) == 0){
                    if(is_extern(table_of_symbol, str)){
                        into_are = "01";
                        char *address = int_to_binary(12, 0);
                    }else{
                        into_are = "10";
                        address = symbol_address(table_of_symbol, str);
                        printf("The adddress of that symbol is : %d\n", address);
                        address_binary = int_to_binary(12, address);
                        printf("The binary adddress of that symbol is : %s\n", address_binary);
                    }
                    add_address_val(table, IC, address_binary, into_are);

                    
                }else{
                    printf("Error!, this label is not defined!\n");
                }
            }
        }
    } 
}

void act_on_istruction_line(int *IC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol, char *source){
    int num_of_operand = 0, num_of_registers = 0;
    int curr_IC = *IC; 
    char *curr_opcode; 
    char *opcode_to_binary, *operand_1 = "", *operand_2 = "", *are = "00";

    for(int i = 0; i <= t->size ; i++){
         
        char *str = trim_comma(t->tokens[i]); 

        // if(strcmp(t->tokens[i], ",") == 0 && flag_comma == 1){ // in case that we have 2 or more commas in a row
        //     printf("Error! too many commas.\n");
        // }
       
        // if( (t->tokens[i+1] == NULL)  && flag_comma == 1){
        //     printf("Error! too many commas.\n");
        // }
        if(strcmp(str, ",") == 0){
            continue;
        }
        if(num_of_operand > 2 || num_of_registers > 2){ // in case that this line is illegal
            printf("Error! no such line!\n");
        }

        if(str_ends_with(str, COLON)){ // it is a label
            if(is_exist_in_symbol_table(table_of_symbol, str) == 0){// remove colon from token!!!
                exit_with_message(str);
                return;
            }
            row_symbol *new_symbol;
            new_symbol = create_symbol_row(remove_colon(str), "code", &curr_IC);
            add_row_to_symbol_table(table_of_symbol ,new_symbol);
        }

        else if(is_opcode(str)) {
            curr_opcode = str;
            (*IC)++;
            opcode_to_binary = trans_opcode_to_binary(str);
        }

        else if(is_register(str)){ // operand that is a register
            if(num_of_operand == 0){
                if(is_op1_to_registers(curr_opcode)){
                    operand_1 = "11";
                }
                else if((num_of_operand == 0) && (is_op2_to_registers(curr_opcode)) ){
                    operand_1 = "00";
                    operand_2 = "11";
                }else{
                    printf("Error! illegal operand for this opcode.\n");
                }        
            }else{
                if(is_op2_to_registers(curr_opcode)){
                     operand_2 = "11";
                }else{
                    printf("Error! illegal operand for this opcode.\n");
                }        
            }
            num_of_operand++;
            num_of_registers++;
            if(num_of_registers < 2){
                (*IC)++;
            }   
        }

        else if(str_begin_with(str, HASH)){ // operand that hash attached to number
            if(num_of_operand == 0){
                if(is_op1_to_hash(curr_opcode)){
                    operand_1 = "00";
                }
                else if((num_of_operand == 0) && (is_op2_to_hash(curr_opcode)) ){
                    operand_1 = "00";
                    operand_2 = "00";
                }else{
                    printf("Error! illegal operand for this opcode.\n");
                }
            } 
            else{
                if(is_op2_to_hash(curr_opcode)){
                    operand_2 = "00";
                }else{
                    printf("Error! illegal operand for this opcode.\n");
                }
            }             
            num_of_operand++;
            (*IC)++; 
        }

        else { // label defined or undefined
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
                        printf("Error! illegal operand for this opcode.\n");
                    } 
                }else{
                    if(is_op2_to_label_array(curr_opcode)){
                        operand_2 = "10";
                    }else{
                     printf("Error! illegal operand for this opcode.\n");
                    }
                }
                num_of_operand++;
            }else{ //just a labels   
                 (*IC)++;
                 if(num_of_operand == 0){
                    if(is_op1_to_label(curr_opcode)){
                        operand_1 = "01";
                    }
                    else if((num_of_operand == 0) && (is_op2_to_label(curr_opcode)) ){
                        operand_1 = "00";
                        operand_2 = "01";
                    }else{
                        printf("Error! illegal operand for this opcode.\n");
                    }
                 }else{
                    if(is_op2_to_label(curr_opcode)){
                        operand_2 = "01";
                    }else{
                        printf("Error! illegal operand for this opcode.\n");
                    }
                }
                num_of_operand++;
            }
        }
    }// end of for
    if(num_of_operand == 0){
        operand_1 = "00";
        operand_2 = "00";
    }
    binary_code *binary_code = create_binary_code(opcode_to_binary, operand_1, operand_2, are, NULL);
    binary_code_table *b_table = create_binary_table_based_on_row_istru(binary_code, t, table_of_symbol);
    
    //initiate table of decimal addresses
    decimal_table *d_table = (decimal_table*)malloc(sizeof(decimal_table));
    d_table->size = 0;
    d_table->decimal_address = NULL;

    int num_of_da = (*IC) - curr_IC; // num of rows 

    for (int i = 0; i < num_of_da; i++)
    {
        add_row_to_decimal_table(d_table, curr_IC++); // TODO: understand whas is this number;
    }
    add_row_to_table_of_operations(table, create_operation_row(d_table, source, b_table, create_explanation_table()));
}

void act_on_directive_line(int *DC, int *da, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol){
    int curr_DC; 
    int num_of_da, flag_data = 0, flag_strintg = 0, flag_extern = 0;
    
    for (int i = 0; i <= t->size; i++){
        if(str_ends_with(t->tokens[i], COLON)){ // it is alabel
            if(is_exist_in_symbol_table(table_of_symbol, t->tokens[i]) == 0){// remove colon from token!!!
                exit_with_message(t->tokens[i]);                
                return;
            }
            curr_DC = *DC;
            row_symbol *new_symbol;
            new_symbol = create_symbol_row(remove_colon(t->tokens[i]), "data", &curr_DC);
            add_row_to_symbol_table(table_of_symbol ,new_symbol);
        }
        else if(is_directive(t->tokens[i])){
            if(strcmp(t->tokens[i], ".data") == 0){
                flag_data = 1;
                continue;
            }
            else if(strcmp(t->tokens[i], ".string") == 0){
                flag_strintg = 1;
                continue;
            }
            else if(strcmp(t->tokens[i], ".extern") == 0){
                flag_extern = 1;
                continue;
            }else{ // entry
                continue;
            }
        }
        else if(flag_data == 1){ // it is data directive
            int flag_comma = 0;

            binary_code *code_binary = create_binary_code(NULL, NULL, NULL, NULL, t->tokens[i]);
            binary_code_table *b_table = create_binary_table_based_on_row_direc_data(code_binary, t, table_of_symbol, i);
            //initiate table of decimal addresses
            decimal_table *d_table = (decimal_table*)malloc(sizeof(decimal_table));
            d_table->size = 0;
            d_table->decimal_address = NULL;
            for (int k = i; k <= t->size; k++)
            {
                add_row_to_decimal_table(d_table, *da);
                (*da)++;
                (*DC)++; 
            }
            operation_row *data_line = create_operation_row(d_table, "", b_table, create_explanation_table());
            add_row_to_table_of_operations(table, data_line);
            break;
        }
        else if(flag_strintg == 1){ // it is string directive
            char* word = t->tokens[i];
            for (int j = 0; j <= strlen(word); j++)
            {
                binary_code *code_binary;
                binary_code_table *b_table;
                char *dest = malloc(sizeof(char));
                strncpy(dest, word+j, 1); 
                if(strcmp(dest, "\"") == 0 ){
                    continue;
                }
                if(j  == strlen(word)){ // the end of string
                    code_binary = create_binary_code(NULL, NULL, NULL, NULL, "0");
                    b_table = create_binary_table_based_on_row_direc_string(code_binary, "0", table_of_symbol, "string");
                }else{
                    code_binary = create_binary_code(NULL, NULL, NULL, NULL, dest);
                    b_table = create_binary_table_based_on_row_direc_string(code_binary, dest, table_of_symbol, "string");
                }
                //initiate table of decimal addresses
                decimal_table *d_table = (decimal_table*)malloc(sizeof(decimal_table));
                d_table->size = 0;
                d_table->decimal_address = NULL;
                add_row_to_decimal_table(d_table, *da);
                (*da)++;
                operation_row *data_line = create_operation_row(d_table, "", b_table, create_explanation_table());
                add_row_to_table_of_operations(table, data_line);
                (*DC)++; 
            } 
            break;
        }
        else if(flag_extern == 1){ // it is extern directive
                if(is_exist_in_symbol_table(table_of_symbol, t->tokens[i])){
                     row_symbol *new_symbol;
                    new_symbol = create_symbol_row(t->tokens[i], "extern", 0);
                    add_row_to_symbol_table(table_of_symbol ,new_symbol);   
                }else{
                    exit_with_message(t->tokens[i]);
                }                 
        }
        else{ 
            continue;
        } 
    }// end for
}

binary_code_table* create_binary_table_based_on_row_istru(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol){

     //initiate table of binary code
    binary_code_table *b_table = (binary_code_table*)malloc(sizeof(binary_code_table));
    b_table->size = 0;
    // b_table->binary_code = NULL;
    add_row_to_binary_code_table(b_table, code);
    int should_check = 0;  int num_operand = 0; int num_register = 0;
    
    for (int i = 0; i <= t->size; i++){
        
        char *str = trim_comma(t->tokens[i]);
        if(num_operand >= 2){
            continue;
        }
        if(strcmp(str, ",") == 0){
            continue;
        }
        if(str_ends_with(str, COLON)){ // it is a label
            continue;
        }
        else if(is_opcode(str)){
            continue;
        }
        else if(str_begin_with(str, HASH)){
            binary_code *code_binary ;
            char temp;
            char *curr_str = remove_hash(str);
            if(curr_str[0] == '-'){
                curr_str = remove_sub(str);
            }
            if(curr_str[0] == '+'){
                curr_str = remove_plus(str);
            }
            temp = *curr_str;
            if(is_exist_in_symbol_table(table_of_symbol, curr_str) == 0){
                char *address = int_to_binary(12, symbol_address(table_of_symbol, curr_str));
                code_binary = create_binary_code(NULL, NULL, NULL, "00",address);
            }else if(isdigit(temp)){
                char* bin = int_to_binary(12, atoi(str+1));
                code_binary = create_binary_code(NULL, NULL, NULL, "00", bin);
            }
            else{
                printf("Erorr! no such symbol.\n");
                exit(1);
            }
            add_row_to_binary_code_table(b_table, code_binary);    
            num_operand++;   
        }
        else if(is_register(str)){
            binary_code *code_binary;
            if(num_register == 0){
                code_binary = create_binary_code(NULL, trans_register_to_binary(str), "000", "00", NULL);
                num_register++;
                num_operand++;
                should_check = 1;
                add_row_to_binary_code_table(b_table, code_binary);   
            }
            else if(num_register == 1 && num_operand == 1){ // operan 1 and 2 are registers
                num_register++;
                num_operand++;
                if (should_check == 1){
                    b_table->binary_code[i-1]->operand_destination = trans_register_to_binary(str);
                }
            }
            else{ // sec operand is register
                code_binary = create_binary_code(NULL, "000", trans_register_to_binary(str),"00", NULL);
                num_operand++;
                num_register++;
                add_row_to_binary_code_table(b_table, code_binary);   
            }
        }
        else{ // labels
            if(is_label_array(str)){ 
                num_operand++;

                binary_code *code_binary1, *code_binary2; 
                char *curr = exclude_label(str);
                // if(is_exist_in_symbol_table(table_of_symbol, curr) == 0){
                //     char *address = int_to_binary(12, symbol_address(table_of_symbol, curr));
                //     code_binary1 = create_binary_code(NULL, NULL, NULL, "00", address);// are ok ?
                // }else{
                    code_binary1 = create_binary_code(NULL, NULL, NULL, NULL, NULL);
                // }
                char *index = exclude_index_from_label(str);
                char temp = *index;
                if(isdigit(temp)){
                    char* bin = int_to_binary(12, atoi(index));
                    code_binary2 = create_binary_code(NULL, NULL, NULL, "00", bin);
                }else{ // is a symbol
                    if(is_exist_in_symbol_table(table_of_symbol, index) == 0){
                        char *address = int_to_binary(12, symbol_address(table_of_symbol, index));
                        code_binary2 = create_binary_code(NULL, NULL, NULL, "00", address);
                    }else{
                        printf("Erorr! no such symbol.\n");
                        exit(1);
                    }
                }  
                add_row_to_binary_code_table(b_table, code_binary1);
                add_row_to_binary_code_table(b_table, code_binary2);
            }
            else{
                num_operand++;
                binary_code *code_binary;
                // if(is_exist_in_symbol_table(table_of_symbol, str) == 0){
                //         char *into_are = "01";
                //     if(is_extern(table_of_symbol, str)){
                //         char *address = int_to_binary(12, 0);
                //         code_binary = create_binary_code(NULL, NULL ,NULL, into_are, address);
                //     }else{
                        // char *address = int_to_binary(12, symbol_address(table_of_symbol, str));
                        // code_binary = create_binary_code(NULL, NULL, NULL, "10", address);
                //     } 
                // }else{ //label not defined yet!
                    code_binary = create_binary_code(NULL, NULL, NULL, NULL, NULL);
                // }
                add_row_to_binary_code_table(b_table, code_binary);
            }
        }
    }// end of for
    return b_table;
}

binary_code_table* create_binary_table_based_on_row_direc_data(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol, int num){
     //initiate table of binary code
    binary_code_table *b_table = (binary_code_table*)malloc(sizeof(binary_code_table));
    b_table->size = 0;
    // b_table->binary_code = NULL;
    // add_row_to_binary_code_table(b_table, code);
    char *bin, *curr_str;
    char temp;

    for (int i = 0; i <= t->size; i++){
        char *str = trim_comma(t->tokens[i]);

        // if(strcmp(str, ",") == 0 && flag_comma == 1){
        //     printf("Error! too many commas.\n");
        // }else{
        //     flag_comma = 0;
        // }
        // if( (t->tokens[i+1] == NULL)  && flag_comma == 1){
        //     printf("Error! too many commas.\n");
        // }
        if(strcmp(str, ",") == 0){
            continue;
        }     
        if(str_ends_with(str, COLON)){ // it is a label
        continue;
        }
        if(is_directive(str)){
            continue;
        }
        if(str[0] == '-'){
            curr_str = remove_sub(str);
        }
        else if(str[0] == '+'){
            curr_str = remove_plus(str);
        }else{
            curr_str = str;
        }
        temp = *curr_str;
            
        if(isdigit(temp)){
            bin = int_to_binary(14, atoi(str));
        }else if(is_exist_in_symbol_table(table_of_symbol, str) == 0){ // is have to be a symbol
            bin = int_to_binary(14, symbol_address(table_of_symbol, str));
        }else{
            printf("Error! this label is not exists.\n");
            exit(1);
        }
            if(i == num ){
            code->address = bin;
            add_row_to_binary_code_table(b_table, code);
            continue;
        }
        binary_code *code_binary = create_binary_code(NULL, NULL, NULL, NULL, bin);
        add_row_to_binary_code_table(b_table, code_binary);
    } //end for   
    return b_table;
}

binary_code_table* create_binary_table_based_on_row_direc_string(binary_code * code, char *str, symbol_table *table_of_symbol, char *cmd){
     //initiate table of binary code
    binary_code_table *b_table = (binary_code_table*)malloc(sizeof(binary_code_table));
    b_table->size = 0;
    char *bin;
    if(cmd == "string"){
        for (int i = 0; i < strlen(str); i++)
        {
            if(strcmp(str, "\"") == 0){
                continue;
            }
            if(isalpha(str[0])){
                bin = int_to_binary(14, (int)str[0]);
            }
            if(strcmp(str, "0") == 0){
                bin = int_to_binary(14, 0);
            }
        }        
    }
     binary_code *code_binary = create_binary_code(NULL, NULL, NULL, NULL, bin);
    add_row_to_binary_code_table(b_table, code_binary);
    return b_table;
}
 
int is_op1_to_hash(char *string){
   for (int i = 0; i < 4; i++)
    {
        if(strcmp(op1_to_hash[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

int is_op2_to_hash(char *string){
   for (int i = 0; i < 2; i++)
    {
        if(strcmp(op2_to_hash[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

int is_op1_to_registers(char *string){
    for (int i = 0; i < 4; i++)
    {
        if(strcmp(op1_to_registers[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

int is_op2_to_registers(char *string){
    for (int i = 0; i < 14; i++)
    {
        if(strcmp(op2_to_registers[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

int is_op1_to_label(char *string){
    for (int i = 0; i < 5; i++)
    {
        if(strcmp(op1_to_label[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

int is_op2_to_label(char *string){
    for (int i = 0; i < 14; i++)
    {
        if(strcmp(op2_to_label[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

int is_op1_to_label_array(char *string){
    for (int i = 0; i < 5; i++)
    {
        if(strcmp(op1_to_label_array[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}

int is_op2_to_label_array(char *string){
    for (int i = 0; i < 11; i++)
    {
        if(strcmp(op2_to_label_array[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}
 
int is_register(char *string){
    for (int i = 0; i < 8; i++)
    {
        if(strcmp(registers[i], string) == 0){
            return 1;
        }
    }
    return 0;
}

int is_directive(char *guidance_cmd){
    for (int i = 0; i < 4; i++)// fix the size of opcode array 
    {
        if(strcmp(guidance[i], guidance_cmd) == 0){
            return 1;
        }    
    }
    return 0;
}

int is_opcode(char *code){
    for (int i = 0; i < 16; i++)// fix the size of opcode array 
    {
        if(strcmp(opcode[i], code) == 0){
            return 1;
        }  
    }
    return 0;
}

char* trans_opcode_to_binary(char *code){
    int num_opcode;
     for (int i = 0; i < 16; i++)// fix the size of opcode array 
    {
        if(strcmp(opcode[i], code) == 0){
            num_opcode = i;
        }  
    }
    return opcode_to_binary[num_opcode];
}

char* trans_register_to_binary(char *code){    
    int num_opcode;
     for (int i = 0; i < 8; i++)// fix the size of opcode array 
    {
        if(strcmp(registers[i], code) == 0){
            num_opcode = i;
        }  
    }
    return register_to_binary[num_opcode];
}

FILE* read(char *path) {
    FILE * fp;
    fp = fopen(path, "r");
    return fp;
}

int main(int argc, char* argv[]){

    //initiate table of operations
    table_of_operations *operations_table = (table_of_operations*)malloc(sizeof(table_of_operations));
    operations_table->size = 0;
    operations_table->rows = NULL;

    //initiate table of symbols
    symbol_table *table_of_symbols = (symbol_table*)malloc(sizeof(symbol_table*));
    table_of_symbols->size = 0;
    table_of_symbols->rows = NULL;

    int *IC;
    int *DC;
    int *da ;

    int a = 100;
    int b = 0;
    int c = 100;

    IC = &a;
    DC = &b;
    da = &c;

    // FILE *f = read(argv[1]);
    // FILE * fp;
    // char line[81];
    // while (fgets(line, sizeof(line), f)){
    //     analyze_line(operations_table, code_binary, table_of_symbols, split(line), IC, DC);
    // }
    // fclose(f);

    char input1[] = ".define sz   =   2";
    char input2[] = "MAIN:     mov  r3 , LIST[sz]";
    char input3[] = "LOOP:     jmp  L1"; 
    char input4[] = "          prn  #-5";
    char input5[] = "          mov  STR[5], STR[2]";
    char input6[] = "          sub  r1, r4";
    char input7[] = "          cmp  r3, #sz" ;
    char input8[] = "          bne  END";
    char input9[] = "L1:       inc  K";
    char input10[] = "         bne   LOOP";
    char input11[] = "END:     stop";
    char input12[] = ".define len = 4";
    char input13[] = "STR:     .string \"abcdef\"";
    char input14[] = "LIST: .data   6, -9, len";
    char input15[] = "K:    .data   22";






    analyze_line(operations_table, table_of_symbols, split(input1), IC, DC, input1, da);
    analyze_line(operations_table, table_of_symbols, split(input2), IC, DC, input2, da);
    analyze_line(operations_table, table_of_symbols, split(input3), IC, DC, input3, da);
    analyze_line(operations_table, table_of_symbols, split(input4), IC, DC, input4, da);
    analyze_line(operations_table, table_of_symbols, split(input5), IC, DC, input5, da);
    analyze_line(operations_table, table_of_symbols, split(input6), IC, DC, input6, da);
    analyze_line(operations_table, table_of_symbols, split(input7), IC, DC, input7, da);
    analyze_line(operations_table, table_of_symbols, split(input8), IC, DC, input8, da);
    analyze_line(operations_table, table_of_symbols, split(input9), IC, DC, input9, da);
    analyze_line(operations_table, table_of_symbols, split(input10), IC, DC, input10, da);
    analyze_line(operations_table, table_of_symbols, split(input11), IC, DC, input11, da);
    analyze_line(operations_table, table_of_symbols, split(input12), IC, DC, input12, da);
    analyze_line(operations_table, table_of_symbols, split(input13), IC, DC, input13, da);
    analyze_line(operations_table, table_of_symbols, split(input14), IC, DC, input14, da);
    analyze_line(operations_table, table_of_symbols, split(input15), IC, DC, input15, da);

    change_val(table_of_symbols, IC); 
    print_symbol_table(table_of_symbols);
    print_table_of_operations(operations_table);

    int size_IC = (*IC) - 100;
    int size_DC = *DC;
    printf("number of instructions and directive  are: %d, %d\n", size_IC ,size_DC);
    
    int d = 100;
    IC = &d; 
    analyze_line_secondly(operations_table, table_of_symbols, split(input1), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input2), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input3), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input4), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input5), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input6), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input7), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input8), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input9), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input10), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input11), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input12), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input13), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input14), IC);
    analyze_line_secondly(operations_table, table_of_symbols, split(input15), IC);
    print_table_of_operations(operations_table);

    return 0;

}