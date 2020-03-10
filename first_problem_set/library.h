#ifndef operating_systems_library
#define operating_systems_library

#include "ctype.h"
#include <stdarg.h>
#include <stdio.h>
#include "stdlib.h"
#include "string.h"

typedef struct decisive_operation 
{
    int length;
    char * operation
} decisive_operation;

typedef struct operation_block 
{
    int number_of_decisive_blocks;
    decisive_operation * decisive_operations_array;
} operation_block;

typedef struct operation_block_array 
{
    int size;
    operation_block * block_array;
} operation_block_array;

typedef struct files_pair_array 
{
    int count;
    files_pair ** pairs;
} files_pair_array;

typedef struct files_pair 
{
    char * file1;
    char * file2;
    
} files_pair;


operation_block_array * create_table(int size);
operation_block_array *  compare_pairs(int n , ...);
int number_of_operations(operation_block_array * arr, char ** decisive_operations_array);
void remove_block(operation_block_array * arr, int index);
void remove_operation(operation_block_array * arr, int operation_index, int block_index);



#endif