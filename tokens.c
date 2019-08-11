#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

#define COLON ":"

/*
    split strings with delimiter of space into tokens
*/
tokenized_line* split(char *line) {
    tokenized_line *t;
    int index = 0;
    char delim[] = " ";
    int i;
    char *word;
    char** p;
    char *ptr;
    char* _line = malloc(strlen(line)); 
    strcpy(_line, line);
    t = malloc(sizeof(tokenized_line));
     
    t->size = 0;
    t->tokens = malloc(sizeof(char*));
     
    ptr = strtok(_line, delim);
    
    t->tokens[index] = ptr;
    while(ptr != NULL){
        ptr = strtok(NULL, delim);
        
        if (ptr != NULL) {
            index++;
            word = ptr;
            p = malloc((index + 1) * sizeof(char*));
             
            if (p == NULL) {
                fprintf(stderr, "Failed to allocate memory\n");
                exit(1);
            }

            for ( i = 0; i <= index; i++)
            {
                p[i] = t->tokens[i];
            }
            free(t->tokens);
             
            
            t->tokens = p;
            t->tokens[index] = word;
        }
    }
    t->size = index;
    return t;
}
