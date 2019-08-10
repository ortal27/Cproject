#include "helpers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "tokens.h"
#include "analyzer.h"

#define MAX_CHAR 31

extern int total_alloc;
extern int total_free;

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
        total_alloc++;
        strncpy(dest, string+i, 1);
        if(dest[0] == ':'){
            res = malloc(i*sizeof(char));
            total_alloc++;
            strncpy(res, string, i); 
            free(dest);
            total_free++;
            return res;
        }
        free(dest);
        total_free++;
    }
    return string;
}

/* get pointer to string and remove char if exist .
return pointer to string.*/
char* remove_hash(char* string){
    if (string == NULL) {
        return string;
    }
    return string + 1;
}

/* get pointer to string and remove char if exist .
return pointer to string.*/
char* remove_sub(char* string){
    if (string == NULL) {
        return string;
    }
    return string + 1;
}

/* get pointer to string and remove chars if exist .
return pointer to string.*/
char* exclude_label(char *string){
    char *res;
    int i;
    int len = strlen(string);
    for (i = 0; i < len; i++)
    {
        if (string[i] == '[') {
            res = malloc((i + 1)*sizeof(char));
            strncpy(res, string, len - i);
            res[i] = '\0';
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
        total_alloc++;
        strncpy(dest, string+i, 1);
        if(strcmp(dest, "[") == 0){
            first = i+1;
        }

        if(strcmp(dest, "]") == 0){
            if(first == -1){
                printf("Error! syntax is wrong.\n");
                exit(1);
            }
            sec = i;   
        }
        free(dest);
        total_free++;
    }
    res = (char*)malloc((sec - first)*sizeof(char));
    total_alloc++;
    strncpy(res , string + first, (sec - first));
    return res;
}

/*get integers numbers and trasfer firts number from descimal to binary.
return poiter string. */
char* int_to_binary(int buffer_size, int value){ 
    int c, k;
    char* buffer = malloc((buffer_size + 1) * sizeof(char));
    total_alloc++;
    for (c = buffer_size - 1; c >= 0; c--){
        k = value >> c;
        if (k & 1){
            buffer[buffer_size-c-1] = '1';
        }
        else{
            buffer[buffer_size-c-1] = '0';
        }
    }
    buffer[buffer_size] = '\0';
    return buffer;  
}

/* get pointer to string and remove char if exist .
return pointer to string.*/
char* remove_plus(char *string){
    if (string == NULL) {
        return string;
    }
    return string + 1;

}

/* get pointer to string and remove char if exist .
return pointer to string.*/
char* remove_dot(char *string){
    if (string == NULL) {
        return string;
    }
    return string + 1;
}


/* get pointer to string and remove char if exist .
return pointer to string.*/
char* trim_comma(char* string){
    char *res;
    int i;
    int len = strlen(string);
    for (i = 0; i < len; i++)
    {
        if(string[i] == ',' && i == 0) {
            res = malloc(sizeof(char) * (len - 1));
            total_alloc++;
            strncpy(res, string + 1, (strlen(string) - 1));
        } else if (string[i] == ',' && i == len) {
            string[len - 1] = '\0';
            return string;
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

FILE* openFile(char *path, char* extention, char* mode) {
    FILE * fp;
    char* buffer;
    buffer = malloc(strlen(path) + strlen(extention) + 1);
    total_alloc++;
    sprintf(buffer, "%s.%s", path, extention);
    fp = fopen(buffer, mode);
    return fp;
}

/* reads file line by line into char** */
char **read_file(FILE *file, int max_line_len, int* size){
    char **lines;
    char **linesCopy;
    char line[81];
    char *lineCpy;
    int i = 0;
    int lineSize = 0;
    
    lines = malloc(sizeof(char*));
    total_alloc++;
    if (file == NULL) {
        fprintf(stderr, "Failed to allocate memory, exiting\n");
    }

    while (fgets(line, sizeof(line), file)){
        linesCopy = malloc(sizeof(char*) * (*size + 1));
        total_alloc++;
        for ( i = 0; i < *size; i++)
        {
            linesCopy[i] = lines[i];
        }
        
        lineCpy = malloc(strlen(line) * sizeof(char));
        total_alloc++;
        if (lineCpy == NULL) {
            fprintf(stderr, "Failed to allocate memory for line %s, exiting\n", line);
        }
        strcpy(lineCpy, line);
        lineSize = strlen(lineCpy);
        if (lineCpy[lineSize - 1] == '\n') {
            lineCpy[lineSize - 1] = '\0';
        }
        linesCopy[*size] = lineCpy;
        (*size)++;
        free(lines);
        lines = linesCopy;
    }

    return lines;
    
}

/* copies the given string into manually allocated char* */
char* make_copy(char* from) {
    int i;
    int len = strlen(from)+1;
    char* copy = malloc(sizeof(char) * len);
    total_alloc++;
    for ( i = 0; i < len; i++)
    {
        copy[i] = from[i];
    }
    
    copy[len] = '\0';
    return copy;
}