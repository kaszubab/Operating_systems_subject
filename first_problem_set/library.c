#include "library.h"


operation_block_array * create_table(int size) 
{
    if (size <= 0) 
    {
        printf("Size of a table must be a positive integer!");
        return NULL;
    }
    operation_block_array * main_array;
    main_array = (operation_block_array *) 
        calloc(1, sizeof(operation_block_array));
    main_array -> size = size;
    main_array -> block_array = (operation_block **) 
        calloc(size, sizeof(operation_block *));
    return main_array;
}

files_pair * make_pair(char * argument) 
{
    char * colon_pos = strchr(argument, ':');

    if (colon_pos == NULL || 
        colon_pos != strrchr(argument, ':'))
    {
        printf("Wrong input format on: %s", argument);
        return NULL;
    }

    int first_file = colon_pos - argument;



    files_pair * pair = (files_pair * )
        calloc(1, sizeof(files_pair));

    pair->file1 = (char * ) calloc(first_file + 1, sizeof(char));
    pair->file2 = (char * ) calloc(strlen(argument) - first_file, sizeof(char));

    strncpy(pair->file1, argument,first_file);
    strcpy(pair->file2, argument+first_file+1);

    return pair;
}


operation_block * prepare_block(char * file_name)
{
    FILE * file_pointer;

    file_pointer = fopen(file_name, "r");

    if (file_pointer == NULL) 
    {
        printf("File %s can't be openned right now", file_name);
        return NULL;
    }

    char * line = NULL;
    size_t line_size = 0;
    decisive_operation * operation;



    operation_block * block;
    block = (operation_block *)
        calloc(1, sizeof(operation_block));

    block->number_of_decisive_blocks = 0; 
    

    while (getline(&line, &line_size, file_pointer) >= 0)
    {
        if (isdigit(line[0]))
        {
            block->number_of_decisive_blocks++;

            if (block->number_of_decisive_blocks == 1)
            {
                block->decisive_operations_array = (decisive_operation **)
                    calloc(block->number_of_decisive_blocks, sizeof(decisive_operation *));
                operation = (decisive_operation *)
                    calloc(1, sizeof(decisive_operation));

                operation->operation = (char *) 
                    calloc(strlen(line),sizeof(char));
                operation->length = strlen(line);
                strcpy(operation->operation, line);
            }
            else
            {
                block->decisive_operations_array = (decisive_operation **) 
                    realloc(block->decisive_operations_array, block->number_of_decisive_blocks);

                block->decisive_operations_array[block->number_of_decisive_blocks-2] = operation;
                
                operation = (decisive_operation *)
                    calloc(1, sizeof(decisive_operation));

                operation->length = strlen(line);

                operation->operation = (char *)
                    calloc(strlen(line),sizeof(char));
                strcpy(operation->operation,line);    
            }
        }
        else
        {
            operation->operation = (char *)
                realloc(operation->operation, strlen(operation->operation) + strlen(line));
            operation->length = operation->length + strlen(line);
            strcat(operation->operation, line);

        }
    }

    block->decisive_operations_array[block->number_of_decisive_blocks-1] = operation;

    fclose(file_pointer);
    operation = NULL;
    free(line);

    char * remove;
    remove =  (char *) 
            calloc(6 + strlen(file_name),(sizeof(char)));
            
    strcpy(remove, "rm -f ");
    strcat(remove, file_name);

    system(remove);
    free(remove);
    
    return block;

}



operation_block_array * compare_pairs(int n , ...) 
{
    va_list arguments;
    va_start( arguments, n);

    files_pair ** pairs = (files_pair **)
        calloc(n, sizeof(files_pair *));
   

    for (int i = 0; i < n; i++) 
    {
        pairs[i] = make_pair(va_arg( arguments, char *));
    }

    char * file_name = "tmp";
    char * diff;

    operation_block_array * op_blocks;
        
    op_blocks = create_table(n);

    for (int i = 0; i < n; i++) 
    {
        diff =  (char *) 
            calloc(10 + strlen(pairs[i]->file1) + strlen(pairs[i]->file2) + strlen(file_name),(sizeof(char)));
            
        strcat(diff, "diff ");
        strcat(diff, pairs[i]->file1);
        strcat(diff, " ");
        strcat(diff, pairs[i]->file2);
        strcat(diff, " > ");
        strcat(diff, file_name);

        system(diff);

        operation_block * new_op_block;
        new_op_block = prepare_block(file_name);
        op_blocks->block_array[i] = new_op_block;

    }

    return op_blocks;

}
