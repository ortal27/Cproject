#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

tokenized_line* split(char *line) {
    char* _line = malloc(strlen(line) + 1); 
    strcpy(_line, line);
    tokenized_line *t = (tokenized_line*)malloc(sizeof(tokenized_line*));
    t->size = 0;
    t->tokens = (char**)malloc(sizeof(char*));
    char delim[] = " ";
    int line_size = strlen(_line);
 
    char *ptr = strtok(_line, delim);
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
    t->size = index;
    return t;
}

char* trim_comma(char* string){
    char *res;
    for (int i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
        strncpy(dest, string+i, 1);
        if(strcmp(dest, ",") == 0 && strlen(string) > 1){
            if(i == 0){ // comma is in beginning of word
                res = malloc((strlen(string) - 1)*sizeof(char));
                strncpy(res, string + (i+1), (strlen(string) - 1));
            }
            else{
                res = malloc(i*sizeof(char));
                strncpy(res, string, i);
            }
            return res;      
        }
    }
    return string;
}