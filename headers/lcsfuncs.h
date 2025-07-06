#ifndef LCSFUNCS_H
#define LCSFUNCS_H

// contains all fns related to the LCS and file diffs

int create_lcs(int len_a, char*** a_ref, int len_b, char*** b_ref, char*** lcs_ptr, char*** unique_refs_ptr, int* unique_refs_len_ptr);

unsigned int create_diff(int len_a, char* a[len_a], int len_b, char* b[len_b], int len_lcs, char* lcs[len_lcs], char*** insertions_ptr , uint8_t** bitstring_ptr);

unsigned int decode_diff(int len_a, char* a[len_a], unsigned int num_insertions, char** insertions, uint8_t* bitstring, char*** b_ptr);

void flush_byte_buffer(uint8_t*** bitstring_ptr_ref, uint8_t* byte_buffer_ref, size_t* bitstring_len_ref);

#endif
