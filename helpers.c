#include "helpers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*get pointer  */
int str_ends_with(const char *s, const char *suffix) {
    size_t slen = strlen(s);
    size_t suffix_len = strlen(suffix);

    return suffix_len <= slen && !strcmp(s + slen - suffix_len, suffix);
}

/*get tow pointeres to strings and check if the second string existing in first string.
return 1 if char exists, else 0. */
int str_begin_with(char *str, char *character){
    int size = strlen(str);
    int i;
    for (i = 0; i < size; i++){
        char can = str[i];
        if(strcmp(&can, character) == 0){
            return 1;
        }
    }
    return 0;
}

/* get pointer to string and check if exist char.
return 1 if exist, else 0.*/
int is_label_array(char *string){
    int count1 = 0;
    int count2 = 0;
    int i;
    for (i = 0; i < strlen(string); i++)
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

/* get pointer to string and remove char if exist .
return pointer to string.*/
char* remove_colon(char *string){
    char *res;
    int i;
    for (i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
        strncpy(dest, string+i, 1);
        if(strcmp(dest, ":") == 0){
            res = malloc(i*sizeof(char));
            strncpy(res, string, i); 
            return res;
        }
    }
    return string;
}

/* get pointer to string and remove char if exist .
return pointer to string.*/
char* remove_hash(char* string){
    char *res;
    int i;
    for (i = 0; i < strlen(string); i++)
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

/* get pointer to string and remove char if exist .
return pointer to string.*/
char* remove_sub(char* string){
    char *res;
    int i;
    for (i = 0; i < strlen(string); i++)
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

/* get pointer to string and remove chars if exist .
return pointer to string.*/
char* exclude_label(char *string){
    char *res;
    int i;
    for (i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
        strncpy(dest, string+i, 1);
        if(strcmp(dest, "[") == 0){
            res = malloc(i*sizeof(char));
            strncpy(res, string, i); 
            return res;
        }
    }
    return string;
}

/* get pointer to string and remove chars if exist .
return pointer to string.*/
char* exclude_index_from_label(char *string){
    int first = -1, sec = -1;
    char *res;
    int i;
    for (i = 0; i < strlen(string); i++)
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

/*get integers numbers and trasfer firts number from descimal to binary.
return poiter string. */
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

/* get pointer to string and remove char if exist .
return pointer to string.*/
char* remove_plus(char *string){
    char *res;
    int i;
    for (i = 0; i < strlen(string); i++)
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

/* get pointer to string and remove char if exist .
return pointer to string.*/
char* remove_dot(char *string){
    char *res;
    int i;
    for (i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
        strncpy(dest, string+i, 1);
        if(strcmp(dest, ".") == 0){
            res = malloc((strlen(string) - 1)*sizeof(char));
            strncpy(res, string + (i+1), (strlen(string) - 1));
            return res;
        }
    }
     return string;
}

/* get pointer to string and remove char if exist .
return pointer to string.*/
char* remove_end_of_line(char *string){
    char *res;
    int i;
    for (i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
        strncpy(dest, string+i, 1);
        if(strcmp(dest, "\n") == 0){
            res = malloc(i*sizeof(char));
            strncpy(res, string, i); 
            return res;
        }
    }
     return string;
}
/* get pointer to string and remove char if exist .
return pointer to string.*/
char* trim_comma(char* string){
    char *res;
    int i;
    for (i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
        strncpy(dest, string+i, 1);
        if(strcmp(dest, ",") == 0 && strlen(string) > 1){
            if(i == 0){ /*  comma is in beginning of word*/
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

char* convert_negative_num(char *address){
   /*  int  i;
    int mask = 00000000000001;
    int mask2 = mask;
    int num = (int)*address;
    char* res;
    for (i = 0; i < 14; i++)
    {
        *res = (num ^ mask);
        mask = mask << 1;
    }
    *res = *res | mask2;*/
    return address;
}

