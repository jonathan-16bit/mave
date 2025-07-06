/*
 * File: lcsfuncs.c
 * Author: Jonathan Eapen (jonathan-16bit)
 * Year: 2025
 * License: MIT License
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "strfuncs.h"

// cute max fn
int max(int a, int b){
    return (a > b)? a : b;
}

// returns num of strings in lcs, modifies input char*** lcs pointer
int create_lcs(int len_a, char*** a_ref, int len_b, char*** b_ref, char*** lcs_ptr, char*** unique_refs_ptr, int* unique_refs_len_ptr){
    int dp[len_a + 1][len_b + 1];

    // initial values
    for (int i = 0; i <= len_a; i++)
        dp[i][0] = 0;
    for (int j = 0; j <= len_b; j++)
        dp[0][j] = 0;

    // filling the dp table
    for (int i = 1; i <= len_a; i++){
        for (int j = 1; j <= len_b; j++){
            if (newstrcmp((*a_ref)[i - 1], (*b_ref)[j - 1]) == 0)
                dp[i][j] = 1 + dp[i - 1][j - 1];
            else
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
        }
    }

    // constructing the LCS
    int i = len_a, j = len_b, size = 0;
    *lcs_ptr = NULL;
    while ((i != 0) && (j != 0)){
        if (newstrcmp((*a_ref)[i - 1], (*b_ref)[j - 1]) == 0){

            // add this word to the LCS
            size += 1;

            // removing duplicate and passing common reference
            *lcs_ptr = str_arr(*lcs_ptr, (*a_ref)[i - 1], size);
            char* holdb = (*b_ref)[j - 1];
            (*b_ref)[j - 1] = (*a_ref)[i - 1];
            free(holdb);
            holdb = NULL;

            if (*lcs_ptr == NULL){
                printf("Error in allocating LCS\n");
                return -1;
            }

            i-- ;
            j-- ;
        }
        else{
            if (dp[i - 1][j] >= dp[i][j - 1]){
                i-- ;
            }
            else if(dp[i - 1][j] < dp[i][j - 1]){
                j-- ;
            }
        }
    }

    // loop through A and B to collect unique refs
    int newflag_a = 1, newflag_b = 1;
    for (int a_idx = 0; a_idx < len_a; ++a_idx){
        for (int idx = 0; idx < (*unique_refs_len_ptr); ++idx){
            if ((*unique_refs_ptr)[idx] == (*a_ref)[a_idx]) newflag_a = 0;
        }
        if (newflag_a == 1){
            (*unique_refs_len_ptr) += 1;
            char** ref_temp = realloc(*unique_refs_ptr, sizeof(char*) * (*unique_refs_len_ptr));
            if (ref_temp == NULL){
                printf("Error reallocing ref_temp in create_lcs\n");
                return -1;
            }
            *unique_refs_ptr = ref_temp;
            (*unique_refs_ptr)[*unique_refs_len_ptr - 1] = (*a_ref)[a_idx];
        }
        newflag_a = 1;
    }

    for (int b_idx = 0; b_idx < len_b; ++b_idx){
        for (int idx = 0; idx < (*unique_refs_len_ptr); ++idx){
            if ((*unique_refs_ptr)[idx] == (*b_ref)[b_idx]) newflag_b = 0;
        }
        if (newflag_b == 1){
            (*unique_refs_len_ptr) += 1;
            char** ref_temp = realloc(*unique_refs_ptr, sizeof(char*) * (*unique_refs_len_ptr));
            if (ref_temp == NULL){
                printf("Error reallocing ref_temp in create_lcs\n");
                return -1;
            }
            *unique_refs_ptr = ref_temp;
            (*unique_refs_ptr)[*unique_refs_len_ptr - 1] = (*b_ref)[b_idx];
        }
        newflag_b = 1;
    }


    // reversing the array
    char* temp = NULL;
    for (int i = 0; i < size / 2; i++){
        temp = *(*lcs_ptr + i);
        *(*lcs_ptr + i) = *(*lcs_ptr + size - 1 - i);
        *(*lcs_ptr + size - 1 - i) = temp;
    }

    return size;
}

void set_bit(uint8_t* byte, uint8_t bit, int pos){
    if (bit == 1)
        *byte |= (1 << pos);
    else if (bit == 0)
        *byte &= ~(1 << pos);
}

// appends (sets) 2 bits as per the mode
void append_byte_buffer(uint8_t* buffer, int buff_size, int mode){
    if (mode == 0){
        set_bit(buffer, 0, 7 - buff_size);
        set_bit(buffer, 0, 7 - buff_size - 1);
    }
    else if (mode == 1){
        set_bit(buffer, 0, 7 - buff_size);
        set_bit(buffer, 1, 7 - buff_size - 1);
    }
    else if (mode == 2){
        set_bit(buffer, 1, 7 - buff_size);
        set_bit(buffer, 0, 7 - buff_size - 1);
    }
    else if (mode == 3){
        set_bit(buffer, 1, 7 - buff_size);
        set_bit(buffer, 1, 7 - buff_size - 1);
    }
}

// appends a new byte to the input bitstring
void append_bitstring(uint8_t** bitstring_ptr, uint8_t append_byte, size_t new_size){
    uint8_t* temp = realloc(*bitstring_ptr, new_size);
    if (temp == NULL){
        printf("Error reallocing bitstring_ptr\n");
        return;
    }
    *bitstring_ptr = temp;
    *(*bitstring_ptr + new_size - 1) = append_byte;
}

// flushes into bitstring and resets vars
void flush_byte_buffer(uint8_t*** bitstring_ptr_ref, uint8_t* byte_buffer_ref, size_t* bitstring_len_ref){
    *bitstring_len_ref += 1;
    append_bitstring(*bitstring_ptr_ref, *byte_buffer_ref, *bitstring_len_ref);
    *byte_buffer_ref = 0;
}

// input: A and B strings, insertions and bitstring ptrs
// output: modified pointers and return len of insertions string
unsigned int create_diff(int len_a, char* a[len_a], int len_b, char* b[len_b], int len_lcs, char** lcs, char*** insertions_ptr, uint8_t** bitstring_ptr){

    // NOTATION:
    // i:A, j:B, k:LCS
    int i = 0, j = 0, k = 0;

    // flags indicate when the resp counters reach max len
    int flag_b = 0, flag_lcs = 0;

    uint8_t byte_buffer = 0;
    size_t buff_size = 0, bitstring_len = 0;
    *insertions_ptr = NULL;
    *bitstring_ptr = NULL;
    unsigned int num_insertions = 0;

    if (len_lcs == 0){
        while (!(flag_b)){

            if ((i == len_a)) i = len_a - 1;
            if ((j == len_b)) j = len_b - 1;

            append_byte_buffer(&byte_buffer, buff_size, 1);
            buff_size += 2;

            if (buff_size == 8){
                flush_byte_buffer(&bitstring_ptr, &byte_buffer, &bitstring_len);
                buff_size = 0;
            }

            append_byte_buffer(&byte_buffer, buff_size, 2);
            buff_size += 2;
            num_insertions += 1;
            *insertions_ptr = str_arr(*insertions_ptr, b[j], num_insertions);

            if (i + 1 <= len_a) i += 1;
            if (j + 1 <= len_b) j += 1;

            if (j == (len_b)) flag_b = 1;

            if (buff_size == 8){
                flush_byte_buffer(&bitstring_ptr, &byte_buffer, &bitstring_len);
                buff_size = 0;
            }
        }

        // appending stop bits
        append_byte_buffer(&byte_buffer, buff_size, 3);

        // flushing any remaining bits
        bitstring_len += 1;
        append_bitstring(bitstring_ptr, byte_buffer, bitstring_len);

        return num_insertions;
    }
    
    // where all are non-empty
    while (!(flag_b && flag_lcs)){

        // when either of i, j are not exhausted yet
        if ((i == len_a) && (len_a != 0)) i = len_a - 1;
        if ((j == len_b) && (len_b != 0)) j = len_b - 1;

        // resetting k in case i, j havent ended yet
        if ((k == len_lcs) && (len_lcs != 0)) k = len_lcs - 1;

        // lcs match found
        if ((newstrcmp(a[i], lcs[k]) == 0) && (newstrcmp(b[j], lcs[k]) == 0)){
            append_byte_buffer(&byte_buffer, buff_size, 0);
            buff_size += 2;
            if (i + 1 <= len_a) i += 1;
            if (j + 1 <= len_b) j += 1;
            if (k + 1 <= len_lcs) k += 1;
        }

        // delete from A
        else if ((newstrcmp(a[i], lcs[k]) == 1) && (newstrcmp(b[j], lcs[k]) == 0)){
            append_byte_buffer(&byte_buffer, buff_size, 1);
            buff_size += 2;
            if (i + 1 <= len_a) i += 1;
        }

        // insert from B
        else if ((newstrcmp(a[i], lcs[k]) == 0) && (newstrcmp(b[j], lcs[k]) == 1)){
            append_byte_buffer(&byte_buffer, buff_size, 2);
            buff_size += 2;
            num_insertions += 1;
            *insertions_ptr = str_arr(*insertions_ptr, b[j], num_insertions);
            if (j + 1 <= len_b) j += 1;
        }

        // delete and insert, lcs not exhausted 
        else if ((newstrcmp(a[i], lcs[k]) == 1) && (newstrcmp(b[j], lcs[k]) == 1)){

            append_byte_buffer(&byte_buffer, buff_size, 1);
            buff_size += 2;

            // flushing buffer
            if (buff_size == 8){
                flush_byte_buffer(&bitstring_ptr, &byte_buffer, &bitstring_len);
                buff_size = 0;
            }

            append_byte_buffer(&byte_buffer, buff_size, 2);
            buff_size += 2;
            num_insertions += 1;
            *insertions_ptr = str_arr(*insertions_ptr, b[j], num_insertions);

            if (i + 1 <= len_a) i += 1;
            if (j + 1 <= len_b) j += 1;
        }

        // checking and updating flags for each iteration
        if (j == (len_b)) flag_b = 1;
        if (k == (len_lcs)) flag_lcs = 1;

        // flush buffer into the bitstring
        if (buff_size == 8){
            flush_byte_buffer(&bitstring_ptr, &byte_buffer, &bitstring_len);
            buff_size = 0;
        }
    }

    // appending stop bits
    append_byte_buffer(&byte_buffer, buff_size, 3);

    // flushing any remaining bits
    bitstring_len += 1;
    append_bitstring(bitstring_ptr, byte_buffer, bitstring_len);
    
    return num_insertions;
}

// input: A, insertions and bitstring, B ptr
// output: modified B ptr and len of B
unsigned int decode_diff(int len_a, char* a[len_a], unsigned int num_insertions, char** insertions, uint8_t* bitstring, char*** b_ptr){

    unsigned int new_size = 0, a_idx = 0, ins_idx = 0;
    *b_ptr = NULL;

    int idx = 0, shift_h = 7, shift_l = 6;

    uint8_t hbit, lbit;
    hbit = (bitstring[idx] >> 7) & 1;
    lbit = (bitstring[idx] >> 6) & 1;

    while (!((hbit == 1) && (lbit == 1))){

        if ((hbit == 0) && (lbit == 0)){
            new_size += 1;
            *b_ptr = str_arr(*b_ptr, a[a_idx], new_size);
            a_idx += 1;
        }

        else if ((hbit == 0) && (lbit == 1)){
            a_idx += 1;
        }

        else if ((hbit == 1) && (lbit == 0)){
            new_size += 1;
            *b_ptr = str_arr(*b_ptr, insertions[ins_idx], new_size);
            ins_idx += 1;
        }

        // updating bit positions for next read
        shift_h = ((shift_h - 2) % 8);
        if (shift_h < 0) shift_h += 8;

        shift_l = ((shift_l - 2) % 8);
        if (shift_l < 0) shift_l += 8;

        if ((shift_h == 7) && (shift_l == 6)) idx++ ;

        hbit = (bitstring[idx] >> shift_h) & 1;
        lbit = (bitstring[idx] >> shift_l) & 1;
    }
    return new_size;
}
