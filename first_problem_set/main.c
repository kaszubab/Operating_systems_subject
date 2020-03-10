#include "library.h"


int main() 
{
    operation_block_array * op;
    op = compare_pairs(2, "plik1.txt:plik2.txt", "plik3.txt:plik4.txt");
    printf( "%s \n", op->block_array[0]->decisive_operations_array[0]->operation );

    
    return 0;
}





