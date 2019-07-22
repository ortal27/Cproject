#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMA 80
#define MACRO ".define"
#define COLON ":"
#define DOT "."
#define HASH "#"


typedef struct{
    char *opcode;
    char *operand_origin;
    char *operand_destination;
}binary_code;

typedef struct{
    char* decimal_address;
    char* sourse_code;
    binary_code *binary_code;
}operation_row;

typedef struct{
    operation_row **rows;
    int size ;
}table_of_operations;


typedef struct {
    int *size; //number of tokens
    char **tokens; // tokens
} tokenized_line ;


typedef struct { /*struct for the symbol table */
     char *symbol;
     char *property;
     int *val;
} row_symbol;

typedef struct {
    row_symbol **rows;
    int size;
} symbol_table;

char *opcode[16] = {"mov", "cmp", "add", "sub", "not", "clr", "lea", "inc", "dec", "jmp", "bne", "red",
"prn", "jsr", "rts", "stop"}; 

char *guidance[4] = {".data", ".string", ".entry", ".extern"};

char *registers[8] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

/****************** declerations ***********************************************************************/
void analyze_line(symbol_table *table_of_symbol, tokenized_line *t, int *IC);
void on_macro(symbol_table *table_of_symbol, char ** words);
tokenized_line* split(char *line);
int str_begin_with(char * string, char *character);
int str_ends_with(const char *s, const char *suffix);
// char* remove_colon(char *string);

int is_exist_in_symbol_table(symbol_table *table_of_symbol, char *symbol);
row_symbol* create_symbol_row(char *symbol, char *property, int *val);
void add_row_to_symbol_table(symbol_table *table_of_symbol ,row_symbol *row);
void print_symbol_table(symbol_table *table_of_symbol);
void exit_with_message(char *symbol);
int is_opcode(char *code);
int is_directive(char *directive_cmd);
void istruction_line(int *IC, char **words, symbol_table *table_of_symbol);
void directive_line(int *IC, char **words, symbol_table *table_of_symbol);
operation_row* create_operation_row(char *decimal_address, char* sourse_code, binary_code *binary_code);
void add_row_to_table_of_operations(table_of_operations *table_of_operations ,operation_row *row);
void print_table_of_operations(table_of_operations *table_of_operations);
binary_code* create_binary_code(char *opcode, char *operand_origin, char *operand_destination);
char *binary_to_string(binary_code *code);
int is_empty_line(char **words);
int is_register(char *string);

/****************** functions ***********************************************************************/
 int str_ends_with(const char *s, const char *suffix) {
    size_t slen = strlen(s);
    size_t suffix_len = strlen(suffix);

    return suffix_len <= slen && !strcmp(s + slen - suffix_len, suffix);
}

// char* remove_colon(char *string){
//     for (int i = 0; i < strlen(string); i++)
//     {
//         if(strcmp(string[i], COLON) == 0){ // ?
//            string[i] = '\0';
//         }
//     }
//     return string;
// }


int str_begin_with(char *str, char *character){
    int size = strlen(str);
    for (int i = 0; i < size; i++){
        char can = str[i];
        printf("the char is : %c\n", str[i]);
        if(strcmp(&can, character) == 0){
            printf("done!\n");
            return 1;
        }
    }
    return 0;
    
}

int is_empty_line(char **words){
    for (int i = 0; i < strlen(*words); i++)
    {
        if((words[i] != "\t") || (words[i] != "\n")){
            return 1;
        }
    }
    return 0;
}

tokenized_line* split(char *line) {
    tokenized_line *t = (tokenized_line*)malloc(sizeof(tokenized_line*));
    t->size = 0;
    t->tokens = (char**)malloc(sizeof(char*));
    char delim[] = " ";
    int line_size = strlen(line);
 
    char *ptr = strtok(line, delim);
    int index = 0;
    t->tokens[index] = ptr;
    while(ptr != NULL){
    ptr = strtok(NULL, delim);
        if (ptr != NULL) {
            index++;
            char *word = ptr;
            t->tokens = (char**) realloc(t->tokens, (index + 1) * sizeof(char*));
            t->tokens[index] = word;
        }
    }
    t->size = &index;
    return t;
}

void analyze_line(symbol_table *table_of_symbol, tokenized_line *t,int *IC){
    int size = *t->size;
    char *token = t->tokens[0];
    char *token2 = t->tokens[1]; 
    char *macro = MACRO;
    if(strcmp(token, ";")== 0){
        printf("the assembler ignoring from this line.\n");
        return;
    }
     if(is_empty_line(t->tokens)){
        printf("this is ampty line.\n");
        return;
    }
    if(strcmp(token,macro) == 0){ 
        on_macro(table_of_symbol,t->tokens);
        return;
    }
    
    printf(" here\n");
    if(is_opcode(token) || is_opcode(token2)){
        printf("its instruction line : %s\n", token2);
        istruction_line(IC, t->tokens,table_of_symbol);
    }

    if(str_begin_with(token, DOT) || str_begin_with(token2,DOT)){
        if(is_directive(token2)){
            printf("its directive line : %s\n", token2);
            directive_line(IC, t->tokens,table_of_symbol);
        }
    }
    //else ?
}

void istruction_line(int *IC, char **words,symbol_table *table_of_symbol){
    for (int i = 0; i < strlen(*words); i++)
    {
        int num_of_registers = 0;
        if(str_ends_with(words[i], COLON)){
            printf("there is label in this line\n");
            if(is_exist_in_symbol_table(table_of_symbol, words[i]) == 0){// remove colon from token!!!
                exit_with_message(words[i]);
                printf("going out!\n");
                return;
            }
            row_symbol *new_symbol;
            new_symbol = create_symbol_row(words[i], "code", IC);
            add_row_to_symbol_table(table_of_symbol ,new_symbol);
            print_symbol_table(table_of_symbol);
            IC++;
        }
        if(str_begin_with(words[i], HASH)){
            printf("***\n");
            IC++;// save address for the number after the hash 
        }
        if(is_register(words[i])){
            num_of_registers++;
            if(num_of_registers < 2){
                IC++;
            }
        }
        //if(is_label_defined(words[i]))

        //if(words[i] is label_array)    
    } 
}

void directive_line(int *IC, char **words, symbol_table *table_of_symbol){
    for (int i = 0; i < strlen(*words); i++){
        if(str_ends_with(words[i], COLON)){
            printf("there is label in this line\n");
            if(is_exist_in_symbol_table(table_of_symbol, words[i]) == 0){// remove colon from token!!!
                exit_with_message(words[i]);
                printf("going out!\n");
                return;
            }
            row_symbol *new_symbol;
            new_symbol = create_symbol_row(words[i], "data", IC);
            add_row_to_symbol_table(table_of_symbol ,new_symbol);
            print_symbol_table(table_of_symbol);
            IC++;
        }
        if(is_directive(words[i])){
            if(strcmp(words[i], ".data")){
                while(i<strlen(*words)){
                    IC++;
                    i++;
                }
                return;//out of this func ?
            }
            if(strcmp(words[i], ".string")){
                while(i<strlen(*words)){
                    IC++;
                    i++;
                }
                return;//out of this func ?
            }
            if(strcmp(words[i], ".entry")){
                //need to torn on a flag
            }
            if(strcmp(words[i], ".extern")){
                //need to torn on a flag
            }
        }
        printf("Eerror! no such directive command.\n");
        return;
    }
}

binary_code* create_binary_code(char *opcode, char *operand_origin, char *operand_destination){
    binary_code *new_binary_code;
    new_binary_code = (binary_code*)malloc(sizeof(binary_code));
    if(!new_binary_code){
        printf("Cannot allocate memory!\n");
        exit(1);
    }
    new_binary_code->opcode = opcode;
    new_binary_code->operand_destination = operand_destination;
    new_binary_code->operand_origin = operand_origin;
    return new_binary_code;
}

operation_row* create_operation_row(char *decimal_address, char* sourse_code, binary_code *binary_code){
    operation_row *new_row;
    new_row = (operation_row*)malloc(sizeof(operation_row));
    if(!new_row){
        printf("Cannot allocate memory!\n");
        exit(1);
    }
    new_row->decimal_address = decimal_address;
    new_row->sourse_code = sourse_code; 
    new_row->binary_code = binary_code;
}

void add_row_to_table_of_operations(table_of_operations *table_of_operations ,operation_row *row){
    // there is no rows in the table yet, add the first row
    if (table_of_operations->rows == NULL) {
        table_of_operations->rows = (operation_row**)malloc(sizeof(operation_row));
    } else {
        table_of_operations->rows = (operation_row**)realloc(table_of_operations->rows, 
        sizeof(row_symbol*) * table_of_operations->size);
    }
    
    if(table_of_operations->rows[table_of_operations->size] == NULL){
        table_of_operations->rows[table_of_operations->size] = row;
    }
    table_of_operations->size++;
}

//TODO: ADD A,R,E
char* binary_to_string(binary_code *code){
    char *buffer = (char*)malloc(14 * sizeof(char));
    char *are = "00";
    sprintf(buffer, "0000%s%s%s%s", code->opcode, code->operand_origin, code->operand_destination, are);
    return buffer;
}

void print_table_of_operations(table_of_operations *table_of_operations){
     for (int i = 0; i < table_of_operations->size; i++)
     {
         char *decimal_address = table_of_operations->rows[i]->decimal_address;
         char *sourse_code = table_of_operations->rows[i]->sourse_code;
         char *opcode = table_of_operations->rows[i]->binary_code->opcode;
         char *dest = table_of_operations->rows[i]->binary_code->operand_destination;
         char *origin = table_of_operations->rows[i]->binary_code->operand_origin;


         printf("operation row is: decimal_address %s, sourse_code %s binary_code %s.\n",decimal_address, sourse_code, binary_to_string(table_of_operations->rows[i]->binary_code));
     }
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
        }    }
    return 0;
}

int is_opcode(char *code){
    for (int i = 0; i < 16; i++)// fix the size of opcode array 
    {
        if(strcmp(opcode[i], code) == 0){
            return 1;
        }    }
    return 0;
}


void on_macro(symbol_table *table_of_symbol, char ** words){
    char *curr_macro = words[1];
    if(is_exist_in_symbol_table(table_of_symbol, curr_macro) == 0){
        exit_with_message(curr_macro);
    }

    row_symbol * row;
    int  val = atoi(words[3]);
    row = create_symbol_row(curr_macro, "macro",&val);
    add_row_to_symbol_table(table_of_symbol, row);   
}

int is_exist_in_symbol_table(symbol_table *table_of_symbol, char *symbol){
    int index = 0;
    int size = table_of_symbol->size;
    for(int i = 0; i < size; i++){
        row_symbol *row = table_of_symbol->rows[i];
        char *row_symbol = row->symbol;
        if(strcmp(row_symbol, symbol) == 0){
            return 0;
        }
    }
    return 1;
}

row_symbol* create_symbol_row(char *symbol, char *property, int* val){
    row_symbol *new_row;
    new_row = (row_symbol*)malloc(sizeof(row_symbol));
    if(!new_row){
        printf("Cannot allocate memory!\n");
        exit(1);
    }
    new_row->symbol = symbol;
    new_row->property = property; 
    new_row->val = val;
    return new_row;
}

void add_row_to_symbol_table(symbol_table *table_of_symbol, row_symbol *row){
    // there is no rows in the table yet, add the first row
    if (table_of_symbol->rows == NULL) {
        table_of_symbol->rows = (row_symbol**)malloc(sizeof(row_symbol*));
    } else {
        table_of_symbol->rows = (row_symbol**)realloc(table_of_symbol->rows, sizeof(row_symbol*) * table_of_symbol->size);
    }
    if(table_of_symbol->rows[table_of_symbol->size] == NULL){
        table_of_symbol->rows[table_of_symbol->size] = row;
    }    
    table_of_symbol->size++;   
}
 
 void print_symbol_table(symbol_table *table_of_symbol){
     for (int i = 0; i < table_of_symbol->size; i++)
     {
         char *symbol = table_of_symbol->rows[i]->symbol;
         char *property = table_of_symbol->rows[i]->property;
         int *val = table_of_symbol->rows[i]->val;

         printf("row symbol is: symbol %s, property %s, val %d.\n", symbol, property, *val);
     }
     
 }
 
void exit_with_message(char *symbol){
    printf("Error, %s already exsist in symbol table\n", symbol);
    exit(1);
}

int main(){
    //initiate table of operations
    table_of_operations *operations_table = (table_of_operations*)malloc(sizeof(table_of_operations));
    operations_table->size = 0;
    operations_table->rows =  NULL;


    operation_row *row = create_operation_row("100", "code", create_binary_code("1111","001", "101" ));
    add_row_to_table_of_operations(operations_table, row);

    print_table_of_operations(operations_table);

    //initiate table of symbols
    symbol_table *table_of_symbols = (symbol_table*)malloc(sizeof(symbol_table*));
    table_of_symbols->size = 0;
    table_of_symbols->rows = NULL;

    char input1[] = ".define    sz   =   2";
    char input2[] = "LOOP: jmp   #"; 
    char input3[] = ".define    a   =   2";
    char input4[] = "str:    .string 'abc' ";
    char input5[] = ";         ,fhfh";
    char input6[] = "     \t   \n";

    int IC = 100;
    
    analyze_line(table_of_symbols, split(input1), &IC);
    analyze_line(table_of_symbols, split(input2), &IC);
    analyze_line(table_of_symbols, split(input3), &IC);
    analyze_line(table_of_symbols, split(input4), &IC);
    analyze_line(table_of_symbols, split(input5), &IC);
    analyze_line(table_of_symbols, split(input6), &IC);


    print_symbol_table(table_of_symbols);
    return 0;
}

