#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMA 80
#define MACRO ".define"
#define colon ":"

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


/****************** declerations ***********************************************************************/
void analyze_line(symbol_table *table_of_symbol, tokenized_line *t);
void on_macro(symbol_table *table_of_symbol, char ** words);
tokenized_line* split(char *line);
int str_ends_with(const char *s, const char *suffix);
int is_exist_in_symbol_table(symbol_table *table_of_symbol, char *symbol);
row_symbol* create_row(char *symbol, char *property, int *val);
void add_row_to_symbol_table(symbol_table *table_of_symbol ,row_symbol *row);
void print_symbol_table(symbol_table *table_of_symbol);
// TODO: add function exit with message
//printf("Error, %s already exsist in symbol table\n", curr_macro);
      //  exit(1);
int is_opcode(char *code);
/****************** functions ***********************************************************************/
 int str_ends_with(const char *s, const char *suffix) {
    size_t slen = strlen(s);
    size_t suffix_len = strlen(suffix);

    return suffix_len <= slen && !strcmp(s + slen - suffix_len, suffix);
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

void analyze_line(symbol_table *table_of_symbol, tokenized_line *t){
    int size = *t->size;
    char *token = t->tokens[0];
    char * macro = MACRO;
    if(strcmp(token,macro) == 0){ // is macro?
        on_macro(table_of_symbol,t->tokens);
        return;
    }
    if(str_ends_with(token, colon)){
        printf("ther is lable in this line\n");
        if(is_exist_in_symbol_table(table_of_symbol, token) == 0){// remove colon from token!!!
            printf("Error, %s already exsist in symbol table\n", token);
            exit(1);
        }
        char *token2 = t->tokens[1]; 
        if(is_opcode(token2)){
            printf("its instruction line\n");
        }
    }
}

int is_opcode(char *code){
    for (int i = 0; i < strlen(*opcode); i++)
    {
        if(strcmp(opcode[i], code)){
            return 1;
        }
    }
    return 0;
}


void on_macro(symbol_table *table_of_symbol, char ** words){
    char *curr_macro = words[1];
    if(is_exist_in_symbol_table(table_of_symbol, curr_macro) == 0){
        printf("Error, %s already exsist in symbol table\n", curr_macro);
        exit(1);
    }

    row_symbol * row;
    int  val = atoi(words[3]);
    row = create_row(curr_macro, "macro",&val);
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

row_symbol* create_row(char *symbol, char *property, int* val){
    row_symbol *new_row;
    new_row = (row_symbol*)malloc(sizeof(row_symbol));
    if(!new_row){
        printf("Cannot allocate memory!\n");
        exit(1);
    }
    new_row->symbol = symbol;
    new_row->property = property; 
    new_row->val = val;
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



int main(){

    //initiate table of symbols
    symbol_table *table_of_symbols = (symbol_table*)malloc(sizeof(symbol_table*));
    table_of_symbols->size = 0;
    table_of_symbols->rows = NULL;

    char input1[] = ".define    sz   =   2";
    char input2[] = "LOOP: jmp L1"; 
    char input3[] = ".define    a   =   2";

    analyze_line(table_of_symbols, split(input1));
    analyze_line(table_of_symbols, split(input2));
    analyze_line(table_of_symbols, split(input3));
    print_symbol_table(table_of_symbols);
    return 0;
}

