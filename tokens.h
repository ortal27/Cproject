#ifndef TOKENS_H_
#define TOKENS_H_


typedef struct {
    int size; /* number of tokens*/
    char **tokens; /*  tokens*/
} tokenized_line ;

tokenized_line* split(char *line);


#endif