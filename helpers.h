#ifndef HELPERS_H_
#define HELPERS_H_


int str_begin_with(char * string, char *character);
int str_ends_with(const char *s, const char *suffix);
// int is_empty_line(char **words);
int is_label_array(char *string);
char* remove_colon(char *string);
char* remove_hash(char *string);
char* remove_sub(char *string);
char* remove_plus(char *string);
char* exclude_label(char *string);
char* exclude_index_from_label(char *string);
char* int_to_binary(int buffer_size, int value);
char* remove_end_of_line(char *string);
char* trim_comma(char* string);
char* convert_negative_num(char *address);


#endif
    