#include "helpers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int str_ends_with(const char *s, const char *suffix) {
    size_t slen = strlen(s);
    size_t suffix_len = strlen(suffix);

    return suffix_len <= slen && !strcmp(s + slen - suffix_len, suffix);
}


int str_begin_with(char *str, char *character){
    int size = strlen(str);
    for (int i = 0; i < size; i++){
        char can = str[i];
        if(strcmp(&can, character) == 0){
            return 1;
        }
    }
    return 0;
}
int is_empty_line(char **words){
    for (int i = 0; i < strlen(*words); i++)
    {
        if( (strcmp(words[i], "\t") != 0) || (strcmp(words[i], "\n") != 0) ){
            return 0;
        }
    }
    return 1;
}

int is_label_array(char *string){
    int count1 = 0;
    int count2 = 0;
    for (int i = 0; i < strlen(string); i++)
    {
        if(string[i] == '['){
            count1++;
        }
        if(string[i] == ']'){
            count2++;
        }
    }
    if(count1 == 1 && count2 == 1){
        return 1;
    }
    return 0;
}

char* remove_colon(char *string){
    char *res;
    for (int i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
        strncpy(dest, string+i, 1);
        if(strcmp(dest, ":") == 0){
            res = malloc(i*sizeof(char));
            strncpy(res, string, i); 
            return res;
        }
    }
}

char* remove_hash(char* string){
    char *res;
    for (int i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
        strncpy(dest, string+i, 1);
        if(strcmp(dest, "#") == 0){
            res = malloc((strlen(string) - 1)*sizeof(char));
            strncpy(res, string + (i+1), (strlen(string) - 1));
            return res;
        }
    }
    return string;
}

char* remove_sub(char* string){
    char *res;
    for (int i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
        strncpy(dest, string+i, 1);
        if(strcmp(dest, "-") == 0){
            res = malloc((strlen(string) - 1)*sizeof(char));
            strncpy(res, string + (i+1), (strlen(string) - 1));
            return res;
        }
    }
     return string;
}

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

char* remove_plus(char *string){
    char *res;
    for (int i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
        strncpy(dest, string+i, 1);
        if(strcmp(dest, "+") == 0){
            res = malloc((strlen(string) - 1)*sizeof(char));
            strncpy(res, string + (i+1), (strlen(string) - 1));
            return res;
        }
    }
     return string;
}