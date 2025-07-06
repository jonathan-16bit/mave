/*
 * File: commitfuncs.c
 * Author: Jonathan Eapen (jonathan-16bit)
 * Year: 2025
 * License: MIT License
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "commitfuncs.h"
#include "lcsfuncs.h"
#include "strfuncs.h" 

int update_b_refs(char**** b_refs_ptr, int commit_count, char** b){
    char*** temp = realloc(*b_refs_ptr, sizeof(char**) * commit_count);
    if (temp == NULL){
        printf("Error allocing temp in update_b_refs\n");
        return 1;
    } 
    *b_refs_ptr = temp;
    (*b_refs_ptr)[commit_count - 1] = b;

    return 0;
}

// creates and returns a pointer to a commit message
char* create_commit_message(int max_message_len){
    char* commit_message = malloc(sizeof(char) * max_message_len);
    if (commit_message == NULL){
        printf("Mem alloc failed for commit message");
        return NULL;
    }
    printf("Enter commit message: ");

    fgets(commit_message, sizeof(char) * max_message_len, stdin);
    remnewl_nullterm(&commit_message, max_message_len); 

    unsigned int trimmed_buffer_size = newstrlen(commit_message) + 1;
    commit_message = realloc(commit_message, trimmed_buffer_size);

    return commit_message;
}

// NOTE: throughout the fns, "b" generally refers to the file contents' (char**) pointer of the current/new commit, and "a" to that of the previous commit 

// appends a new commit to the linked list of commits, given the commit_message and b_ptr
void create_commit_ptr(commit_t** head_ptr_ref, int identifier, char* commit_message, char** b, int len_b, char*** unique_refs_ptr, int* unique_refs_len_ptr){
    
    commit_t* interm_ptr = *head_ptr_ref;
    while (((interm_ptr -> next) != NULL) && ((interm_ptr -> identifier) < identifier))
        interm_ptr = interm_ptr -> next;

    // new commit object, zero initialized
    commit_t* commit_ptr = malloc(sizeof(commit_t));
    if (commit_ptr == NULL){
        printf("Mem alloc for new block ptr failed\n");
        return;
    }
    *commit_ptr = (commit_t){0};
    
    interm_ptr -> next = commit_ptr;
    commit_ptr -> prev = interm_ptr;

    commit_ptr -> identifier = identifier;
    commit_ptr -> commit_message = commit_message;

    // temporary A string array for creation from the head pointer (the snapshot)
    char** tempa = (*head_ptr_ref) -> snapshot;
    int len_tempa = (*head_ptr_ref) -> len_snapshot;

    // to traverse till the current commit to successively reconstruct the actual A needed
    char*** tempb_arr = NULL;
    int tempb_count = 0;

    char** tempb = NULL;
    int len_tempb = 0;
    while (((commit_ptr -> next) != NULL) && ((commit_ptr -> identifier) != (identifier))){
        len_tempb = decode_diff(len_tempa, tempa, commit_ptr -> num_insertions, commit_ptr -> insertions, commit_ptr -> bitstring, &tempb);

        tempb_count += 1;
        char*** temp = realloc(tempb_arr, sizeof(char**) * tempb_count);
        if (temp == NULL){
            printf("Error reallocing tempb_arr\n");
            return;
        }
        tempb_arr = temp;
        tempb_arr[tempb_count - 1] = tempb;

        tempa = tempb;
        len_tempa = len_tempb;

        commit_ptr = commit_ptr -> next;
    }

    // so far the A string array (for the new commit) has been generated
    // creating the LCS from A and B
    char** lcs = NULL;
    // MODIFIED
    int len_lcs = create_lcs(len_tempa, &tempa, len_b, &b, &lcs, unique_refs_ptr, unique_refs_len_ptr);

    // generating bitstring and insertions string array
    uint8_t* bitstring = 0;
    char** insertions = NULL;
    unsigned int num_insertions = create_diff(len_tempa, tempa, len_b, b, len_lcs, lcs, &insertions, &bitstring);

    commit_ptr -> bitstring = bitstring;
    commit_ptr -> insertions = insertions;
    commit_ptr -> num_insertions = num_insertions;


    // freeing intermediate mallocd vars
    for (int i = 0; i < tempb_count; ++i){
        free(tempb_arr[i]);
        tempb_arr[i] = NULL;
    }
    free(tempb_arr);
    tempb_arr = NULL;

    free(lcs);
    lcs = NULL;

    return;
}

void load_commit(commit_t** head_ptr_ref, unsigned int identifier, char* filename){

    // starting from the first commit
    commit_t* commit_ptr = *head_ptr_ref;

    // intermediate vars for A
    char** tempa = (*head_ptr_ref) -> snapshot;
    int len_tempa = (*head_ptr_ref) -> len_snapshot;

    // in case head_ptr is passed, write immediately 
    if (identifier == 1){
        write_to_file(filename, tempa, len_tempa);
        return;
    }

    // intermediate vars for B
    char** tempb = NULL;
    int len_tempb = 0;

    commit_ptr = commit_ptr -> next;

    // to keep track of vars to be freed
    char*** tempb_arr = NULL;
    int tempb_count = 0;

    while ((commit_ptr -> identifier) <= identifier){
        len_tempb = decode_diff(len_tempa, tempa, commit_ptr -> num_insertions, commit_ptr -> insertions, commit_ptr -> bitstring, &tempb);

        tempb_count += 1;
        char*** temp = realloc(tempb_arr, sizeof(char**) * tempb_count);
        if (temp == NULL){
            printf("Error reallocing tempb_arr\n");
            return;
        }
        tempb_arr = temp;
        tempb_arr[tempb_count - 1] = tempb;

        tempa = tempb;
        len_tempa = len_tempb;

        if ((commit_ptr -> next) == NULL) break;
        commit_ptr = commit_ptr -> next;
    }
    
    // write commit's file contents
    write_to_file(filename, tempa, len_tempa);

    // freeing intermediate vars
    for (int i = 0; i < tempb_count; ++i){
        free(tempb_arr[i]);
        tempb_arr[i] = NULL;
    }
    free(tempb_arr);
    tempb_arr = NULL;

    return;
}

void memclean_commits(commit_t** head_ptr_ref, int num_commits){
    commit_t* commit_ptr = *head_ptr_ref;

    while ((commit_ptr) != NULL){

        // freeing fields independent of the file content string arrays
        free(commit_ptr -> commit_message);
        free(commit_ptr -> bitstring);
        free(commit_ptr -> insertions);

        // freeing commit ptrs 
        if ((commit_ptr -> prev) != NULL){
            free(commit_ptr -> prev);
            commit_ptr -> prev = NULL;
        }

        if ((commit_ptr -> next) == NULL){
            free(commit_ptr);
            break;
        }

        commit_ptr = commit_ptr -> next;
    }

    return;
}
