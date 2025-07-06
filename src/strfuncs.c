/*
 * File: strfuncs.c
 * Author: Jonathan Eapen (jonathan-16bit)
 * Year: 2025
 * License: MIT License
*/


#include <stdio.h>
#include <stdlib.h>
#include "strfuncs.h"

#define MAX_LINE_SIZE 1024

// checks if the input character reps a single digit
int is_chardigit(char ch){
    unsigned int n = ch - '0';
    return ((0 <= n) && (n <= 9));
}

// fun little code (so much for readability)
int newstrlen(char* str){
    int len = 0;
    while(*(str + (len++)) != '\0');
    return len - 1;
}

// custom defn
char* newstrncpy(char* des, char* src, int num_bytes){
    for (int i = 0; i < num_bytes; i++)
        *(des++) = *(src++) ;
    return des - num_bytes;
}

// custom defn
int newstrcmp(char* s1, char* s2){
    int len_s1 = newstrlen(s1), len_s2 = newstrlen(s2);
    if (len_s1 != len_s2) return 1;
    for (int i = 0; i < len_s1; i++){
        if (s1[i] != s2[i]) return 1;
    }
    return 0;
}

// very coherent fn name
// modifies the input char* array: removes the first \n and replaces with a \0
void remnewl_nullterm(char** buffer_ptr, unsigned int buff_len){
    unsigned int idx = 0, counter = buff_len;
    char ch = (*buffer_ptr)[0];

    while (counter--){
        if (ch == '\0') return;
        else if (ch == '\n'){
            (*buffer_ptr)[idx] = '\0';
            return;
        }
        idx += 1;
        ch = (*buffer_ptr)[idx];
    }
    return;
}

// does stdin flushing and null termination for input validation
void flush_prompt(char** buffer_ref, int buffer_len, char* prompt){
    int ch = ' ';
    if ((*buffer_ref)[buffer_len - 2] != '\0'){
        while ((ch = getchar()) != '\n' && ch != EOF);
        (*buffer_ref)[buffer_len - 2] = '\0';
    }

    printf("%s", prompt);
    fgets(*buffer_ref, buffer_len, stdin);
    remnewl_nullterm(buffer_ref, buffer_len);

    return;
}

// number of characters in file
int len_file(char* filename){
    FILE* fp = fopen(filename, "r");
    if (fp == NULL){
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    int filelines_len = ftell(fp);
    fclose(fp);

    return filelines_len;
}

// write to file
void write_to_file(char* filename, char** tempa, int len_tempa){
    FILE* fp = fopen(filename, "w");
    if (fp == NULL){
        printf("Error opening file to load commit\n");
        return;
    }
    for (int i = 0; i < len_tempa; ++i){
        fputs(tempa[i], fp);
        if (i != (len_tempa - 1)) 
            fputc('\n', fp);
    }
    fclose(fp);
    return;
}

// returns ptr to array of lines, modifies input var (number of lines)
char** line_ptr_array(char* filename, int* line_count_store){
    FILE *fp;
    fp = fopen(filename, "rb");
    if (fp == NULL){
        printf("Error opening file\n");
        return NULL;
    }

    char buffer[MAX_LINE_SIZE + 1];
    char** lp_arr = NULL;
    int num_lines = 0;

    while (fgets(buffer, MAX_LINE_SIZE, fp) != NULL){    
        int len_buffer = newstrlen(buffer);
        int eff_len = len_buffer;

        if (buffer[len_buffer - 1] == '\n'){
            eff_len--; 
            buffer[len_buffer - 1] = '\0';
        }

        // individual line ptr (that contains contents of the line)
        char* line = malloc(eff_len + 1);
        if (line == NULL){
            printf("Mem alloc failed\n");
            return NULL;
        }
        line = newstrncpy(line, buffer, eff_len + 1);
        num_lines++ ;

        // growing array of line ptrs
        char** temp = realloc(lp_arr, num_lines * sizeof(char*));
        if (temp == NULL){
            printf("Mem realloc failed\n");
            return NULL;
        }
        lp_arr = temp;
        *(lp_arr + num_lines - 1) = line;
    }

    *line_count_store = num_lines;
    fclose(fp);

    return lp_arr;
}

// input:  new element and new size of arr
// output: updated char** arr
char** str_arr(char** arr, char* str, unsigned int new_size){
    char** temp = realloc(arr, sizeof(char*) * new_size);
    if (temp == NULL){
        printf("Mem realloc failed\n");
        return NULL;
    }
    arr = temp;

    *(arr + new_size - 1) = str;
    return arr;
}
