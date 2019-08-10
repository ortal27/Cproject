#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

#define COLON ":"

extern int total_alloc;
extern int total_free;

/*get pointer for line.
anter strings between empty chars into two dimensional array and return pointer for this array. */
tokenized_line* split(char *line) {
    tokenized_line *t;
    char delim[] = " ";
    int index = 0;
    int i;
    char *word;
    char** p;
    char *ptr;
    char* _line = malloc(strlen(line) + 1); 
    strcpy(_line, line);
    t = malloc(sizeof(tokenized_line));
    total_alloc++;
    t->size = 0;
    t->tokens = malloc(sizeof(char*));
    total_alloc++;
    ptr = strtok(_line, delim);
    
    t->tokens[index] = ptr;
    while(ptr != NULL){
        ptr = strtok(NULL, delim);
        
        if (ptr != NULL) {
            index++;
            word = ptr;
            p = malloc((index + 1) * sizeof(char*));
            total_alloc++;
            if (p == NULL) {
                printf("Failed to reallocate memory to tokenise the word %s\n", line);
            }

            for ( i = 0; i < index; i++)
            {
                p[i] = t->tokens[i];
            }
            free(t->tokens);
            total_free++;
            
            t->tokens = p;
            t->tokens[index] = word;
        }
    }
    t->size = index;
    return t;
}

