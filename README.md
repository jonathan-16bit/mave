# mave 
**MAVE** (the *Mostly Alright Versioning Engine*) is a local version control sim written in C.

It uses a **Longest Common Subsequence (LCS)**-based line diffing algorithm to efficiently track file changes in a linear commit workflow.

# Instructions:
1. Clone the repo:  
`git clone`  

2. Compile all files:  
`make`

3. Or, compile and execute at once:  
`make run`

4. Remove the executable/residual object files:  
`make clean`

## Line-based diffing using the LCS algorithm: 

When comparing two consecutive versions of a file (each represented as a `char**`), an LCS algorithm is used to identify the shared lines between commits. These are stored in `char** lcs`, which references pointers to the unchanged lines.

Lines in the new version that do not appear in `lcs` are stored in `char** insertions`, which references pointers to the new lines.

A `uint8_t* bitstring` is generated to encode the reconstruction of the next version. For each line, `bitstring` specifies the source of the string, either `lcs` (common) or `insertions` (new).

The encoded `bitstring` and `insertions` are stored as part of each commit.
