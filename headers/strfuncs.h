#ifndef STRFUNCS_H
#define STRFUNCS_H

// implementations of string fns and other misc stuff

int is_chardigit(char ch);
int newstrlen(char* str);
char* newstrncpy(char* des, char* src, int num_bytes);
int newstrcmp(char* s1, char* s2);
void remnewl_nullterm(char** buffer_ptr, unsigned int buff_len);
void flush_prompt(char** buffer_ref, int buffer_len, char* prompt);

int len_file(char* filename);
void write_to_file(char* filename, char** tempa, int len_tempa);
char** line_ptr_array(char* filename, int* line_count_store);
char** str_arr(char** arr, char* str, unsigned int new_size);

#endif
