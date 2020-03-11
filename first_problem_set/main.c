#include "library.h"


int main() 
{
    operation_block_array * op;
    op = create_table(2);

    char ** files;
    files = (char **) calloc(2, sizeof(char *));
    files[0] = "plik1.txt:plik2.txt";
    files[1] = "plik3.txt:plik4.txt";

    files_pair_array * files_pairs; 
    files_pairs =  make_sequence(files, 2);

    compare_pairs(op, files_pairs);

    printf( "%s \n", op->block_array[0]->decisive_operations_array[0]->operation );
    printf( "%s \n \n", op->block_array[0]->decisive_operations_array[1]->operation );
    
    
    printf( "%s \n", op->block_array[1]->decisive_operations_array[0]->operation );
    printf( "%s \n", op->block_array[1]->decisive_operations_array[1]->operation );

    
    return 0;
}





