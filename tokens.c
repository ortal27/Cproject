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