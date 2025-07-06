#ifndef COMMITFUNCS_H
#define COMMITFUNCS_H

// contains all fns related to the commit blocks in the linked list

typedef struct commit_object{
    int identifier;
    char* commit_message;
    
    char** snapshot;
    int len_snapshot;

    uint8_t* bitstring;
    char** insertions;
    unsigned int num_insertions;

    struct commit_object* prev;
    struct commit_object* next;
}commit_t;

int update_b_refs(char**** b_refs_ptr, int commit_count, char** b);
char* create_commit_message(int max_message_len);
void create_commit_ptr(commit_t** head_ptr_ref, int identifier, char* commit_message, char** b, int len_b, char*** unique_refs_ptr, int* unique_refs_len_ptr);
void load_commit(commit_t** head_ptr_ref, unsigned int identifier, char* filename);
void memclean_commits(commit_t** head_ptr_ref, int num_commits);

#endif
