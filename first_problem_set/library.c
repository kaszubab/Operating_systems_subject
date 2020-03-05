#include "library.h"
#include "stdlib.h"

struct operation_block_array * create_table(int size) 
{
    struct operation_block_array * main_array;
    main_array = (struct operation_block_array *) 
        calloc(size, sizeof(struct operation_block_array)); 
    return main_array;
    
}