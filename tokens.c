#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

#define COLON ":"


tokenized_line* split(char *line) {
    char* _line = malloc(strlen(line) + 1); 
    strcpy(_line, line);
    tokenized_line *t = malloc(sizeof(tokenized_line));
    t->size = 0;
    t->tokens = malloc(sizeof(char*));
    char delim[] = " ";
    char *ptr = strtok(_line, delim);
    int index = 0;
    t->tokens[index] = ptr;
    while(ptr != NULL){
        ptr = strtok(NULL, delim);
        if (ptr != NULL) {
            index++;
            char *word = ptr;
            char** p = realloc(t->tokens, (index + 1) * sizeof(char*));
            if (p == NULL) {
                printf("Failed to reallocate memory to tokenise the word %s\n", line);
            }
            t->tokens = p;
            t->tokens[index] = word;
        }
    }
    t->size = index;
    return t;
}

int check_comma(tokenized_line *t){
    char *curr_str;
    int flag_no_comma = 0;
    int i = 0;
    // while(i <= t->size){
    //      if(str_ends_with(t->tokens[i], COLON)){
    //         i++;
    //     }
    //     else if((is_opcode(t->tokens[i])) || (is_directive(t->tokens[i]) == ".data") ){
    //         curr_str = *t->tokens[i];
    //         flag_no_comma = 1;
    //         i++;   
    //     }
    //     else{
    //         if(strcmp(t->tokens[i], ",") == 0 || strcmp((t->tokens[i])[0], ",") == 0 ){
    //             if((is_opcode(curr_str)) || (is_directive(curr_str) == ".data")){
    //                 return 1; // comma before operand or number
    //             }else if(flag_no_comma == 0){
    //                 return 1; // too mant commas
    //             }else{ // flag_no_comma == 1
    //                 flag_no_comma == 0;
    //                 curr_str = *t->tokens[i];
    //             }

    //         }else{ // no has a comma
    //             if(flag_no_comma == 1 && strcmp(t->tokens[i], ",") == 0)
    //             curr_str = *t->tokens[i];
    //         }
    //         i++;
    //     }
        
}