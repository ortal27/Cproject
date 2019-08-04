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

/****************** declerations ***********************************************************************/
void analyze_line(table_of_operations *table ,symbol_table *table_of_symbol, tokenized_line *t, int *IC, int *DC, char *soruce);
char* remove_colon(char string);
int is_opcode(char *code);
int is_directive(char *directive_cmd);
void act_on_istruction_line(int *IC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol, char *source);
void act_on_directive_line(int *DC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol);

void act_on_istruction_line2(int *IC, tokenized_line *t, symbol_table *table_of_symbol, table_of_operations *table, binary_code_table *code_binary);
void act_on_directive_line2(int *IC, int *DC, tokenized_line *t, symbol_table *table_of_symbol, table_of_operations *table, binary_code_table *code_binary);
int is_register(char *string);
char* trans_opcode_to_binary(char *code);
char* trans_register_to_binary(char *code);

void analyze_line_secondly(table_of_operations *table, symbol_table *table_of_symbol, tokenized_line *t, int *IC, int *DC);
binary_code_table* create_binary_table_based_on_row(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol);
char* method_addressing(symbol_table *table_of_symbol, char *string);
char* int_to_binary(int buffer_size, int value);

char* exclude_label(char *string);
char* exclude_index_from_label(char *string);
/****************** functions ***********************************************************************/
 
char* remove_colon(char string){
    char *new_str;
    new_str = &string;
    int size = strlen(&string);
    printf("size is: %d\n", size);
    for (int i = 0; i < size; i++)
    {
        if(new_str[i] == ':') { // ?
           new_str[i] = '\0';
           printf("string is : %s\n", new_str);
        }
    }
    return new_str;
}

void analyze_line(table_of_operations *table ,symbol_table *table_of_symbol, tokenized_line *t, int *IC, int *DC, char *source){
    int size = t->size;
    char *token = t->tokens[0];
    char *token2 = t->tokens[1]; 
    char *macro = MACRO;
    
    if(strcmp(token, ";")== 0){
        printf("the assembler ignoring from this line.\n");
        return;
    }
    else if(is_empty_line(t->tokens)){ // something is not working!
        printf("this is ampty line.\n");
        return;
    }

    else if(strcmp(token,macro) == 0){ 
        on_macro(table_of_symbol,t->tokens);
        return;
    }
    
    else if((is_opcode(token)) || (is_opcode(token2))){
        act_on_istruction_line(IC, t, table, table_of_symbol, source);
    }

    else if(str_begin_with(token, DOT) || str_begin_with(token2,DOT)){
        if(is_directive(token) || is_directive(token2)){
            act_on_directive_line(DC, t, table, table_of_symbol);
        }
    }else{
        printf("Error! this line is illegal.\n");
        return;
    }
    printf("num of IC is: %d, num of DC is: %d\n", *IC, *DC);
    
    for (int i = 0; i < (table_of_symbol->size); i++)
    {
        if(strcmp(table_of_symbol->rows[i]->property, "data") == 0){
           int value = table_of_symbol->rows[i]->val;
           value = value + (*IC);
        }
    }
}

char* method_addressing(symbol_table *table_of_symbol, char *string){
    if(is_register(string)){
        return "11";
    }
    else if(str_begin_with(string, HASH)){
        return "00";
    }
    else if(is_exist_in_symbol_table(table_of_symbol, string)){
        return "01";
    }
    else{
        return "10";
    }
}

void act_on_istruction_line(int *IC, tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol, char *source){

    
    int size = t->size;
    int num_of_operand = 0, num_of_registers = 0;
    int curr_IC = *IC; 

    char *opcode_to_binary, *operand_1 = "", *operand_2 = "", *are = "00";
    for(int i = 0; i <= size ; i++)
    {
        if(strcmp(t->tokens[i], ",") == 0){
            continue;
        }
        if(num_of_operand > 2 || num_of_registers > 2){
            printf("Error! no such line!\n");
        }
        if(str_ends_with(t->tokens[i], COLON)){ // it is a label
            if(is_exist_in_symbol_table(table_of_symbol, t->tokens[i]) == 0){// remove colon from token!!!
                exit_with_message(t->tokens[i]);
                return;
            }
            row_symbol *new_symbol;
            new_symbol = create_symbol_row((t->tokens[i]), "code", &curr_IC);
            add_row_to_symbol_table(table_of_symbol ,new_symbol);
        }

        else if(is_opcode(t->tokens[i])) {
            (*IC)++;
            opcode_to_binary = trans_opcode_to_binary(t->tokens[i]);

        }

        else if(is_register(t->tokens[i])){ // operand that is a register
            if(num_of_operand == 0){
                    operand_1 = "11";
            } else{
                    operand_2 = "11";
            }
            num_of_operand++;
            num_of_registers++;
            if(num_of_registers < 2){
                (*IC)++;
            }   
        }

        else if(str_begin_with(t->tokens[i], HASH)){ // operand that hash attached to number
            if(num_of_operand == 0){
                operand_1 = "00";
            } else{
                operand_2 = "00";
            }             
            num_of_operand++;
            (*IC)++; 
        }

        else { // label defined or undefined
            if(is_label_array(t->tokens[i])){
                (*IC) = (*IC) + 2;
                if(num_of_operand == 0){
                    operand_1 = "10"; 
                }else{
                    operand_2 = "10";
                }
                num_of_operand++;
            }else{ //just a labels   
                 (*IC)++;
                 if(num_of_operand == 0){
                    operand_1 = "01";
                 }else{
                    operand_2 = "01";
                }
                num_of_operand++;
            }
        }
    }// end of for
    binary_code *binary_code = create_binary_code(opcode_to_binary, operand_1, operand_2, are, NULL);
    binary_code_table *b_table = create_binary_table_based_on_row(binary_code, t, table_of_symbol);
    //initiate table of decimal addresses
    decimal_table *d_table = (decimal_table*)malloc(sizeof(decimal_table));
    d_table->size = 0;
    d_table->decimal_address = NULL;

    for (int i = 0; i < b_table->size; i++)
    {
        add_row_to_decimal_table(d_table, 0); // TODO: understand whas is this number;
    }
    add_row_to_table_of_operations(table, create_operation_row(d_table, source, b_table));
    
}

void act_on_directive_line(int *DC,tokenized_line *t, table_of_operations *table, symbol_table *table_of_symbol){
    int size = t->size;  int curr_DC;
    
    for (int i = 0; i <= size; i++){
        if(str_ends_with(t->tokens[i], COLON)){
            if(is_exist_in_symbol_table(table_of_symbol, t->tokens[i]) == 0){// remove colon from token!!!
                exit_with_message(t->tokens[i]);                
                return;
            }
            curr_DC = *DC;
            row_symbol *new_symbol;
            new_symbol = create_symbol_row(t->tokens[i], "data", &curr_DC);
            add_row_to_symbol_table(table_of_symbol ,new_symbol);
        }
        else{
            if(is_directive(t->tokens[i])){
                if(strcmp(t->tokens[i], ".data") == 0){
                    for (int j = i+1; j <= t->size; j++)
                    {
                        if(strcmp(t->tokens[j], ",") == 0){
                            continue;
                        }
                        binary_code *code_binary = create_binary_code(NULL, NULL, NULL, NULL, t->tokens[j]);
                        binary_code_table *b_table = create_binary_table_based_on_row(code_binary, t, table_of_symbol);
                        //initiate table of decimal addresses
                        decimal_table *d_table = (decimal_table*)malloc(sizeof(decimal_table));
                        d_table->size = 0;
                        d_table->decimal_address = NULL;
                        add_row_to_decimal_table(d_table, 0);
                        operation_row *data_line = create_operation_row(d_table, "", b_table);
                        add_row_to_table_of_operations(table, data_line);
                        (*DC)++; 
                    }
                    break;
                }
                
                
                if(strcmp(t->tokens[i], ".string") == 0){
                    char* word = t->tokens[i+1];
                    for (int j = 0; j <strlen(word); j++)
                    {
                        char *dest = malloc(sizeof(char));
                        strncpy(dest, word+j, 1); 
                        if(strcmp(dest, "\"") == 0){
                            continue;
                        }

                        binary_code *code_binary = create_binary_code(NULL, NULL, NULL, NULL, dest);
                        binary_code_table *b_table = create_binary_table_based_on_row(code_binary, t, table_of_symbol);
                        //initiate table of decimal addresses
                        decimal_table *d_table = (decimal_table*)malloc(sizeof(decimal_table));
                        d_table->size = 0;
                        d_table->decimal_address = NULL;
                        add_row_to_decimal_table(d_table, 0);
                        operation_row *data_line = create_operation_row(d_table, "", b_table);
                        add_row_to_table_of_operations(table, data_line);
                        (*DC)++; 
                    } 
                    break;
                }
                if(strcmp(t->tokens[i], ".entry") == 0){
                    return;// care in second section.
                }
                if(strcmp(t->tokens[i], ".extern") == 0){
                    i++;
                    row_symbol *new_symbol;
                    new_symbol = create_symbol_row(t->tokens[i], "extern", 0);
                    add_row_to_symbol_table(table_of_symbol ,new_symbol);
                    return;
                }
            }
        }
    }// end for
}
/* 
void analyze_line_secondly(table_of_operations *table, symbol_table *table_of_symbol, tokenized_line *t, int *IC, int *DC){


    int size = t->size;
    char *token = t->tokens[0];
    char *token2 = t->tokens[1]; 
    int flag_entry = 0;
    int flag_extern = 0;
    if((is_opcode(token)) || (is_opcode(token2))){
        act_on_istruction_line2(IC, t,table_of_symbol, table, code_binary);
    }

    if(str_begin_with(token, DOT) || str_begin_with(token2,DOT)){
        if(strcmp(token, ".entry") == 0){
            char *str_word = token2;
                if(is_exist_in_symbol_table(table_of_symbol, str_word)){
                    flag_entry = 1;
                    // need to enter to entry file the address for that lable
                }else{
                    printf("Error! no such label.");
                }
        }
        else if(strcmp(token, ".extern") == 0){
            char *str_word = token2;
                if(is_exist_in_symbol_table(table_of_symbol, str_word)){
                    flag_extern = 1;
                    // need to enter to entry file the address for that lable
                }else{
                    printf("Error! no such label.");
                }
        }else{
            act_on_directive_line2(IC ,DC, t ,table_of_symbol, table, code_binary);
        }
    }
}


    
   
void act_on_directive_line2(int *IC, int *DC, tokenized_line *t, symbol_table *table_of_symbol, table_of_operations *table, binary_code_table *code_binary){
    int size = t->size;  int curr_IC = *IC, init_IC = *IC;
    
    for (int i = 0; i <= size; i++){
        if(str_ends_with(t->tokens[i], COLON)){
            continue;
        }
        else{
            if(strcmp(t->tokens[i], ".data") == 0){
                char *str_word = t->tokens[i+1];
                while(strcmp(str_word, "\0") != 0){
                    (*DC)++; 
                    str_word++;
                    curr_IC++;
                }
                break;
            }
            if(strcmp(t->tokens[i], ".string") == 0){
                char *str_word = t->tokens[i+1];
                while(strcmp(str_word+1, "\0") != 0){
                    str_word++;
                    (*DC)++; 
                    curr_IC++;
                }            
                break;
            }
            if(strcmp(t->tokens[i], ".entry") == 0){
                continue;
            }
            if(strcmp(t->tokens[i], ".extern") == 0){
                continue;
            }
        }
    }
}*/


binary_code_table* create_binary_table_based_on_row(binary_code * code, tokenized_line *t, symbol_table *table_of_symbol){

     //initiate table of binary code
    binary_code_table *b_table = (binary_code_table*)malloc(sizeof(binary_code_table));
    b_table->size = 0;
    // b_table->binary_code = NULL;
    int num_operand = 0;

    add_row_to_binary_code_table(b_table, code);
    for (int i = 0; i <= t->size; i++)
    {
        if(strcmp(t->tokens[i], ",") == 0){
            continue;
        }
        if(str_ends_with(t->tokens[i], COLON)){ // it is a label
            continue;
        }
        else if(is_opcode(t->tokens[i])){
            continue;
        }
        else if(str_begin_with(t->tokens[i], HASH)){
            char* bin = int_to_binary(12, atoi(t->tokens[i]+1));
            binary_code *code_binary = create_binary_code(NULL, NULL, NULL, "00", bin);
            add_row_to_binary_code_table(b_table, code_binary);    
            num_operand++;   
        }
        else if(is_register(t->tokens[i])){
            binary_code *code_binary;
            if((num_operand == 0) && is_register(t->tokens[i+1])){ // both operands are registers
                code_binary = create_binary_code(NULL, trans_register_to_binary(t->tokens[i]), trans_register_to_binary(t->tokens[i+1]), "00", NULL);
                num_operand = num_operand + 2;
            }else if(num_operand == 0){ // first operand is register
                code_binary = create_binary_code(NULL, trans_register_to_binary(t->tokens[i]), "000", "00", NULL);
                num_operand++;
            }else{ // sec operand is register
                code_binary = create_binary_code(NULL, "000", trans_register_to_binary(t->tokens[i]),"00", NULL);
                num_operand++;
            }
            add_row_to_binary_code_table(b_table, code_binary);          
        }
        else{
            if(is_label_array(t->tokens[i])){ 
                num_operand++;

                binary_code *code_binary1, *code_binary2; 
                char *curr = exclude_label(t->tokens[i]);
                if(is_exist_in_symbol_table(table_of_symbol, curr) == 0){
                    char *address = int_to_binary(12, symbol_address(table_of_symbol, curr));
                    code_binary1 = create_binary_code(NULL, NULL, NULL, "are", address);// need to return exact value of are!
                }else{
                    code_binary1 = create_binary_code(NULL, NULL, NULL, NULL, NULL);
                }
                char *index = exclude_index_from_label(t->tokens[i]);
                if(isdigit(atoi(index))){
                    char* bin = int_to_binary(12, atoi(index));
                    code_binary2 = create_binary_code(NULL, NULL, NULL, "are", bin);
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
                printf("Adding code 1 %s\n", binary_to_string_first_word(code_binary1));

                add_row_to_binary_code_table(b_table, code_binary2);
                printf("Adding code 2%s\n", binary_to_string_first_word(code_binary2));

            }
            else{
                num_operand++;
                binary_code *code_binary;
                if(is_exist_in_symbol_table(table_of_symbol, t->tokens[i])){
                    char *address = int_to_binary(12, symbol_address(table_of_symbol, t->tokens[i]));
                    code_binary = create_binary_code(NULL, NULL, NULL, "are", address);// need to return exact value of are!
                }else{
                    code_binary = create_binary_code(NULL, NULL, NULL, NULL, NULL);
                }
            }
        }
    }// end of for
    return b_table;
}


    
    

//     int size_operations = *IC - curr_IC; int i = 0;
//     printf("size value is: %d\n", size_operations);

//     if(num_of_operand == 0){ // only opcode cmd
//         operand_1 = "00";
//         operand_2 = "00";
//     }
//     if(operand_2 == ""){
//         operand_2 = operand_1;
//         address_op2 = address_op1;
//         address_op1 = NULL;
//         operand_1 = "00";
//         explanation_op2 = explanation_op1;
//         explanation_op1 = NULL; 
//     }
//            printf("111address of operand2 is: %s\n", address_op2);
    
//     while(i < size_operations){
//         binary_code *line_code;

//         if(i == 0){ // complite line
//              printf("333address of operand2 is: %s\n", address_op2);
//             line_code = create_binary_code(opcode_to_binary, operand_1, operand_2, are, NULL, NULL);
//             printf("222address of operand2 is: %s\n", address_op2);
//             add_row_to_binary_code_table(code_binary, line_code);
//             explanation = "First_word";
//             operation_row *new_row;
//             new_row = create_operation_row(curr_IC, t->tokens[0],explanation, code_binary);
//             add_row_to_table_of_operations(table, new_row);
            

//         }

//         else if(i == 1){ 
//             if(num_of_operand == 2){ 
//                 if(num_of_registers == 2){
//                     line_code = create_binary_code(NULL,NULL, NULL, are, address_op1, address_op2);
//                 }else if( (num_of_registers == 1) && (strcmp(explanation_op1, "register") == 0)){
//                     line_code = create_binary_code(NULL,NULL, NULL, are, address_op1, "000");
//                 }else{
//                     line_code = create_binary_code(NULL,NULL, NULL, are, address_op1, NULL);
//                 }

//                 printf("555address of operand2 is: %s\n", address_op2);

//                 add_row_to_binary_code_table(code_binary, line_code);
//                 operation_row *new_row;
//                 new_row = create_operation_row(curr_IC, t->tokens[0],explanation_op1, code_binary);
//                 add_row_to_table_of_operations(table, new_row);
//             }else{ // have only destination operand
//                 line_code = create_binary_code(NULL,NULL, NULL, are, "000", address_op2);
//                 add_row_to_binary_code_table(code_binary, line_code);
//                 operation_row *new_row;
//                 new_row = create_operation_row(curr_IC, t->tokens[0],explanation_op2, code_binary);
//                 add_row_to_table_of_operations(table, new_row);
//                 printf("66address of operand2 is: %s\n", address_op2);

//             }
//         }
//         else{
//            printf("*****address of operand2 is: %s\n", address_op2);

//             line_code = create_binary_code(NULL,NULL, NULL, are, NULL, address_op2);
//             add_row_to_binary_code_table(code_binary, line_code);
//             operation_row *new_row;
//             new_row = create_operation_row(curr_IC, t->tokens[0],explanation_op2, code_binary);
//             add_row_to_table_of_operations(table, new_row);
//         }
//         curr_IC++;
//         i++;
//     }
//     print_table_of_operations(table);
// }


char* exclude_label(char *string){
    char *res;
    for (int i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
        strncpy(dest, string+i, 1);
        if(strcmp(dest, "[") == 0){
            res = malloc(i*sizeof(char));
            strncpy(res, string, i); 
            return res;
        }
    }
}

char* exclude_index_from_label(char *string){
    int first = -1, sec = -1;
    char *res;
    for (int i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
        strncpy(dest, string+i, 1);
        if(strcmp(dest, "[") == 0){
            first = i+1;
        }
        if(strcmp(dest, "]") == 0){
           if(first == -1){
               printf("Error! syntax are wrong.\n");
               exit(1); 
           }
            sec = i;   
        }
    }
    res = (char*)malloc((sec - first)*sizeof(char));
    strncpy(res , string + first, (sec - first));
    return res;
}

char* int_to_binary(int buffer_size, int value){ 
    char* buffer = malloc(buffer_size * sizeof(char));
    int c, k;
    for (c = buffer_size; c >= 0; c--){
        k = value >> c;
        if (k & 1){
            buffer[buffer_size-c-1] = '1';
        }
        else{
            buffer[buffer_size-c-1] = '0';
        }
    }
    return buffer;
    
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

    int a = 100;
    int b = 0;

    IC = &a;
    DC = &b;

    // FILE *f = read(argv[1]);
    // FILE * fp;
    // char line[81];
    // while (fgets(line, sizeof(line), f)){
    //     analyze_line(operations_table, code_binary, table_of_symbols, split(line), IC, DC);
    // }
    // fclose(f);

    char input1[] = ".define    sz   =   2";
    char input2[] = "LOOP: jmp   #-5"; 
    char input3[] = ".define    a   =   2";
    char input4[] = "str:    .string \"abc\" ";
    char input5[] = ";         ,fhfh";
   // char input6[] = "     \t   \n";
    char input7[] = "    .define  f = 2" ;
    char input8[] = "A: add r1 , r4";
    char input9[] = "MAIN: mov r3 , LIST[sz]";
    char input10[] = ".data 2 , 1";

 
    
    
    analyze_line(operations_table, table_of_symbols, split(input1), IC, DC, input1);
     analyze_line(operations_table, table_of_symbols, split(input2), IC, DC, input2);
    // analyze_line(operations_table, table_of_symbols, split(input3), IC, DC);
    // analyze_line(operations_table, table_of_symbols, split(input4), IC, DC);
    // analyze_line(operations_table, table_of_symbols, split(input5), IC, DC);
    // analyze_line(operations_table, table_of_symbols, split(input8), IC, DC);
     analyze_line(operations_table, table_of_symbols, split(input9), IC, DC, input9);
    // analyze_line(operations_table, table_of_symbols, split(input10), IC, DC);

    print_symbol_table(table_of_symbols);
    print_table_of_operations(operations_table);
//    //printf("the binary code for num 2 is: %s\n", convert_to_binary(1));
//     int size_IC = (*IC) - 100;
//     int size_DC = *DC;
//     printf("number of instructions and directive  are: %d, %d\n", size_IC ,size_DC);
//     *IC = 100; *DC = 0;

    // analyze_line_secondly(operations_table, code_binary, table_of_symbols, split(input2), IC, DC);
    // analyze_line_secondly(operations_table, code_binary, table_of_symbols, split(input8), IC, DC);
    // analyze_line_secondly(operations_table, code_binary, table_of_symbols, split(input9), IC, DC);

    return 0;
}

