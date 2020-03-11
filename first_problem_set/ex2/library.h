#ifndef operating_systems_library
#define operating_systems_library


#define  _POSIX_C_SOURCE 200809L
#include "ctype.h"
#include <stdarg.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef struct decisive_operation 
{
    int length;
    char * operation;
} decisive_operation;

typedef struct operation_block 
{
    int number_of_decisive_blocks;
    decisive_operation ** decisive_operations_array;
} operation_block;

typedef struct operation_block_array 
{
    int size;
    int last_index;
    operation_block ** block_array;
} operation_block_array;


typedef struct files_pair 
{
    char * file1;
    char * file2;
    
} files_pair;

typedef struct files_pair_array
{
    files_pair ** pairs_array;
    int len;
} files_pair_array;



operation_block_array * create_table(int size);
files_pair * make_pair(char * argument);
files_pair_array * make_sequence(char ** argument, int arguments_len);
int prepare_block(char * file_name, operation_block_array * op_blocks);
int compare_pairs(operation_block_array * op_blocks, files_pair_array * files); 
int number_of_operations(operation_block_array * arr, int index);
void remove_block(operation_block_array * arr, int index);
void remove_operation(operation_block_array * arr, int operation_index, int block_index);



#endif