#include "helpers.h"
#include <stdio.h>
#include <string.h>


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