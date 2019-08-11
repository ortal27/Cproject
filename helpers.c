#include "helpers.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "tokens.h"
#include "analyzer.h"

#define MAX_CHAR 31
#define MAX_LINE_LEN 256

/*
    checks whenever given string end with suffix
*/
int str_ends_with(const char *s, const char *suffix) {
    size_t slen = strlen(s);
    size_t suffix_len = strlen(suffix);

    return suffix_len <= slen && !strcmp(s + slen - suffix_len, suffix);
}

/*
    checks whenever given string end with postfix
*/
int str_begin_with(char *str, char *postfix){
    int size = strlen(str);
    int i;
    for (i = 0; i < size; i++){
        char can = str[i];
        if(strcmp(&can, postfix) == 0){
            return 1;
        }
    }
    return 0;
}

/*
    Checks the given array to contain both '[' and ']'
    Makes sure that the order is correct
*/
int is_label_array(char *array){
    int count1 = 0;
    int count2 = 0;
    int i;
    for (i = 0; i < strlen(array); i++)
    {
        if(array[i] == '['){
            count1++;
        }
        if(array[i] == ']'){
            count2++;
        }
    }
    if(count1 == 1 && count2 == 1){
        return 1;
    }
    return 0;
}

/*
    returns copy of the input with deleted colon (:)
*/
char* remove_colon(char *string){
    char *res;
    int i;
    for (i = 0; i < strlen(string); i++)
    {
        char *dest = malloc(sizeof(char));
         
        strncpy(dest, string+i, 1);
        if(dest[0] == ':'){
            res = malloc(i*sizeof(char));
             
            strncpy(res, string, i); 
            free(dest);
             
            return res;
        }
        free(dest);
         
    }
    return string;
}

/*
    returns copy of the input with delete char HASH (#)
*/
char* remove_hash(char* string){
    if (string == NULL) {
        return string;
    }
    return string + 1;
}

/*
    returns pointer to the first elem in string, "deleting" to subtract char (-)
*/
char* remove_sub(char* string){
    if (string == NULL) {
        return string;
    }
    return string + 1;
}

/*
    returns the name of the array from given input
    input = STR[5] => STR as output
    this memory have to be freed
*/
char* exclude_label(char *label){
    char *res;
    int i;
    int len = strlen(label);
    for (i = 0; i < len; i++)
    {
        if (label[i] == '[') {
            res = malloc((i + 1)*sizeof(char));
            strncpy(res, label, len );
            res[i] = '\0';
            return res;
        }
    }
    return label;
}

/*
    returns the index of the array from given input
    input = STR[5] => 5 as output
    this memory have to be freed 
*/
char* exclude_index_from_label(char *string, int *has_error){
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
                fprintf(stderr, "Error! syntax is wrong.\n");
                *has_error = 1;
                return NULL;
            }
            sec = i;   
        }
        free(dest);
         
    }
    res = (char*)malloc((sec - first)*sizeof(char));
     
    strncpy(res , string + first, (sec - first));
    return res;
}

/*
    Convert integer to binary reporesentaiton with given buffer size
    The result memory have to be freed
    In case of number too long, will ignore higher bites
*/
char* int_to_binary(int buffer_size, int value){ 
    int c, k;
    char* buffer = malloc((buffer_size + 1) * sizeof(char));
     
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

/*
    Remove plus char (+) from given input
*/
char* remove_plus(char *string){
    if (string == NULL) {
        return string;
    }
    return string + 1;

}

/*
    Remove dot char (.) from given input
*/
char* remove_dot(char *string){
    if (string == NULL) {
        return string;
    }
    return string + 1;
}


/*
    Removes comma char (,) from input
*/
char* trim_comma(char* string){
    char *res;
    int i;
    int len = strlen(string);
    for (i = 0; i <= len; i++)
    {
        if(string[i] == ',' && i == 0 && string[i + 1] != '\0') {
            res = malloc(sizeof(char) * (len - 1));
             
            strncpy(res, string + 1, (strlen(string) - 1));
            return res;
        } else if (string[i] == ',' && i == len-1) {
            string[len - 1] = '\0';
            return string;
        }
    }
    return string;
}


/*
    Opens a file with given mode
*/
FILE* open_file(char *path, char* extention, char* mode) {
    FILE * fp;
    char* buffer;
    buffer = malloc(strlen(path) + strlen(extention) + 2);
     
    sprintf(buffer, "%s.%s", path, extention);
    fp = fopen(buffer, mode);
    free(buffer);
     
    return fp;
}

/* reads file line by line into char** */
char **read_file(FILE *file, int max_line_len, int* size){
    char **lines;
    char **linesCopy;
    char *line;
    char *lineCpy;
    int i = 0;
    int lineSize = 0;

    line = malloc(sizeof(char) * MAX_LINE_LEN);
     
    
    lines = malloc(sizeof(char*));
     

    while (fgets(line, MAX_LINE_LEN , file)){
        if (strlen(line) > max_line_len) {
            fprintf(stderr, "Line %d is too long, allowed: %lu actual %d , ignoring line \n", *size, strlen(line), max_line_len);
            continue;
        }
        linesCopy = malloc(sizeof(char*) * (*size + 1));
         
        for ( i = 0; i < *size; i++)
        {
            linesCopy[i] = lines[i];
        }
        
        lineCpy = malloc(max_line_len * sizeof(char));
         
        if (lineCpy == NULL) {
            fprintf(stderr, "Failed to allocate memory for line %s, exiting\n", line);
        }
        
        for ( i = 0; i < max_line_len; i++)
        {
            lineCpy[i] = line[i];
        }
        
        lineSize = strlen(lineCpy);
        if (lineCpy[lineSize - 1] == '\n') {
            lineCpy[lineSize - 1] = '\0';
        }
        linesCopy[*size] = lineCpy;
        (*size)++;
        free(lines);
        lines = linesCopy;
    }
    free(line);
     
    return lines;
    
}

/* copies the given string into manually allocated char* */
char* make_copy(char* from) {
    int i;
    int len = strlen(from)+1;
    char* copy = malloc(sizeof(char) * len);
     
    for ( i = 0; i < len; i++)
    {
        copy[i] = from[i];
    }
    
    copy[len] = '\0';
    return copy;
}

/*
    checkes whenever arr includes somewhere string
*/
int includes(const char** arr, int len, char* string) {
    int i;
    for (i = 0; i < len; i++)
    {
        if(strcmp(arr[i], string) == 0){
            return 1;
        }
    }
    return 0; 
}