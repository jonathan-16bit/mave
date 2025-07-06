/*
 * File: main.c
 * Author: Jonathan Eapen (jonathan-16bit)
 * Year: 2025
 * License: MIT License
*/

 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "strfuncs.h"
#include "commitfuncs.h"
#include "lcsfuncs.h"

#define MAX_MESSAGE_LEN 1024
#define MAX_FILENAME_LEN 256
#define MAX_NUM_COMMITS 64 
#define USER_CHOICE_BUF_SIZE 3
#define NUMCHOICE_BUF_SIZE 4

int main(void){
    printf("\n\nMAVE Version Control\n\n");

    // GENERAL COMMIT COUNTER
    int commit_count = 1;

    // HEAD (ORIGINAL COMMIT) POINTER
    commit_t* head_ptr = malloc(sizeof(commit_t));
    *head_ptr = (commit_t){0};
    head_ptr -> identifier = commit_count;
    
    // INITIAL FILE LOAD
    char* filename = malloc(MAX_FILENAME_LEN);
    if (filename == NULL){
        printf("Mem alloc for filename failed\n");
        return 1;
    }

    // FILE VALIDATION
    int filelines_len = 0;
    while (filelines_len == 0){
        printf("Enter (non-empty) text file path to track (with initial commit): ");
        if (fgets(filename, MAX_FILENAME_LEN, stdin)){
            remnewl_nullterm(&filename, MAX_FILENAME_LEN);
            if (newstrlen(filename) == 0){
                printf("Non-empty filename is invalid\n");
                continue;
            }
        }

        filelines_len = len_file(filename);
        if (filelines_len == -1){
            printf("Filepath does not exist\n");
            filelines_len = 0; 
        }
    }
    head_ptr -> snapshot = line_ptr_array(filename, &filelines_len);
    head_ptr -> len_snapshot = filelines_len;

    // FOR MEMORY CLEANUP 
    // to pass references to unique string pointers
    char** unique_refs = NULL;
    int unique_refs_len = 0;

    // to pass references to outer char** (line) pointers
    char*** b_refs = NULL;
    int status = update_b_refs(&b_refs, commit_count, head_ptr -> snapshot);
    if (status) return 1;

    // INITIAL COMMIT MESSAGE
    head_ptr -> commit_message = create_commit_message(MAX_MESSAGE_LEN);
    
    // string buffers and flushing character for input validation
    char* choice_buffer = malloc(sizeof(char) * USER_CHOICE_BUF_SIZE);
    char* numchoice_buffer = malloc(sizeof(char) * NUMCHOICE_BUF_SIZE);

    int choice = -1;
    while (1){
        printf("\n\n1. Create a commit\n2. View all commit IDs and messages\n3. Load a commit by ID\n0. Exit\n");

        // INPUT VALIDATION FOR choice
        for (int i = 0; i < USER_CHOICE_BUF_SIZE; ++i) choice_buffer[i] = '\0' ;
        while (1){

            flush_prompt(&choice_buffer, USER_CHOICE_BUF_SIZE, ">> Choice: ");

            char choice_char = choice_buffer[0];
            if (choice_buffer[1] == '\0'){
                if (choice_char == '0');
                else if (choice_char == '1');
                else if (choice_char == '2');
                else if (choice_char == '3');
                else continue; 
                choice = choice_char - '0';  
                break;  
            }
        }

        // INDIVIDUAL OPTIONS
        if (choice == 1){

            commit_count += 1;
            if (commit_count > MAX_NUM_COMMITS){
                printf("Max number of commits reached\n");
                break;
            }
            printf("Enter non-empty file contents\n");

            for (int i = 0; i < USER_CHOICE_BUF_SIZE; ++i) choice_buffer[i] = '\0' ;
            int len_b = 0, discontinue = 0;
            while (len_b == 0){

                flush_prompt(&choice_buffer, USER_CHOICE_BUF_SIZE, "Enter 'y' to commit, 'n' to discontinue: ");

                if (choice_buffer[1] == '\0' && choice_buffer[0] == 'y'){
                    len_b = len_file(filename);
                    if (len_b == -1){
                        printf("Filepath does not exist\n");
                        len_b = 0;
                    }
                }
                else if (choice_buffer[1] == '\0' && choice_buffer[0] == 'n'){
                    discontinue = 1;
                    commit_count -= 1;
                    break;
                }
                else continue;
            }

            if (discontinue == 1) continue;

            // LOAD IN FROM MEMORY
            char** b = line_ptr_array(filename, &len_b);

            // COMMIT MESSAGE INPUT
            char* commit_message = create_commit_message(MAX_MESSAGE_LEN);

            // ATTACHING NEW COMMIT TO LINKED LIST
            create_commit_ptr(&head_ptr, commit_count, commit_message, b, len_b, &unique_refs, &unique_refs_len);

            // FOR MEMORY CLEANUP
            status = update_b_refs(&b_refs, commit_count, b);
        }

        else if (choice == 2){
            printf("\nCOMMIT METADATA: \n");
            commit_t* view_ptr = head_ptr;
            while (view_ptr != NULL){
                printf("\nCommit Identifier: %d\nCommit Message: %s\n", view_ptr -> identifier, view_ptr -> commit_message);
                view_ptr = view_ptr -> next;
            }
        }

        else if (choice == 3){
            for (int i = 0; i < NUMCHOICE_BUF_SIZE; ++i) numchoice_buffer[i] = '\0' ;
            int commit_choice = commit_count + 1, discontinue = 0;

            while ((commit_choice > commit_count) || (commit_choice == 0)){
                flush_prompt(&numchoice_buffer, NUMCHOICE_BUF_SIZE, "Enter valid commit ID / 'n' to exit: ");

                // this logic is kind of hardcoded
                if (numchoice_buffer[1] == '\0'){
                    if (is_chardigit(numchoice_buffer[0]))
                        commit_choice = numchoice_buffer[0] - '0';
                    else if (numchoice_buffer[0] == 'n'){
                        discontinue = 1;
                        break;
                    }
                }
                else if (numchoice_buffer[2] == '\0'){
                    if (is_chardigit(numchoice_buffer[0]) && is_chardigit(numchoice_buffer[1])){
                        char digit_10 = numchoice_buffer[0] - '0';
                        char digit_1 = numchoice_buffer[1] - '0';
                        commit_choice = (10 * digit_10 + digit_1);
                    }
                }
                else continue;
            }
            if (discontinue == 1) continue;

            load_commit(&head_ptr, commit_choice, filename);
            printf("\nCommit %d has been loaded into %s\n", commit_choice, filename);
        }

        else if (choice != 0){
            printf("Enter valid choice\n");
        }

        // choice is 0
        else{
            int discontinue = 0;
            for (int i = 0; i < USER_CHOICE_BUF_SIZE; ++i) choice_buffer[i] = '\0' ;

            while(1){
                flush_prompt(&choice_buffer, USER_CHOICE_BUF_SIZE, "Enter 'y' to confirm exit, 'n' to cancel: ");

                if (choice_buffer[1] == '\0'){
                    if (choice_buffer[0] == 'y') discontinue = 1;
                    else if (choice_buffer[0] == 'n');
                    else continue;
                    break;
                }
            }

            if (discontinue) break;
            else continue;
        }
    }

    // MEMORY CLEANUP UPON PROGRAM COMPLETION
    // freeing unique internal char*s
    if (commit_count > 1){
        for (int i = 0; i < unique_refs_len; ++i){
            free(unique_refs[i]);
            unique_refs[i] = NULL;
        }
        free(unique_refs);
    }

    // manually handling single-commit case, because the refs are only stored within create_lcs 
    else{
        for (int i = 0; i < (head_ptr -> len_snapshot); ++i){
            free((head_ptr -> snapshot)[i]);
            (head_ptr -> snapshot)[i] = NULL;
        }
    }

    // freeing each source char** (ie, file line contents ptr)
    for (int i = 0; i < commit_count; ++i){
        free(b_refs[i]);
        b_refs[i] = NULL;
    }
    free(b_refs);

    // freeing fields independent of file contents
    memclean_commits(&head_ptr, commit_count);
    free(filename);
    free(choice_buffer);
    free(numchoice_buffer);

    printf("\nExiting MAVE\n\n");
    return 0;
}
