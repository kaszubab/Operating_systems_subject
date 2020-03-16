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
    main_array->last_index = -1;
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


files_pair_array * make_sequence(char ** arguments, int arg_len) 
{
    files_pair_array * files = (files_pair_array *)
        calloc(1, sizeof(files_pair_array));

    files ->pairs_array = (files_pair **) calloc(arg_len, sizeof(char *));

    int pos = 0;

    for (int i = 0; i < arg_len; i++)
    {
        files_pair * file = make_pair(arguments[i]);
        if (file != NULL) 
        {
            files->pairs_array[pos++] = file;
        }
        
    }

    files->len = pos;

    return files;
}


int prepare_block(char * file_name, operation_block_array * op_blocks)
{
    FILE * file_pointer;

    file_pointer = fopen(file_name, "r");

    if (file_pointer == NULL) 
    {
        printf("File %s can't be openned right now", file_name);
        return -1;
    }

    char * line = NULL;
    size_t line_size = 0;
    char * curr_operation;



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
                
                curr_operation = (char *) 
                    calloc(strlen(line) + 1, sizeof(char));

                strcpy(curr_operation,line);
            }
            else
            {
                block->decisive_operations_array = (decisive_operation **) 
                    realloc(block->decisive_operations_array, sizeof(decisive_operation *) * block->number_of_decisive_blocks);

                decisive_operation * operation = (decisive_operation *) 
                    calloc(1, sizeof(decisive_operation));

                operation->length = strlen(curr_operation) + 1;

                operation->operation = (char *)
                    calloc(operation->length, sizeof(char));

                strcpy(operation->operation, curr_operation);

                block->decisive_operations_array[block->number_of_decisive_blocks-2] = operation;
                
                curr_operation = (char *)
                    calloc(strlen(line) + 1, sizeof(char));
 
                strcpy(curr_operation, line);    
            }
        }
        else
        {
            curr_operation = (char *)
                realloc(curr_operation, sizeof(char) * (strlen(curr_operation) + strlen(line) + 1));

            strcat(curr_operation, line);

        }
    }

    decisive_operation * operation = (decisive_operation *) 
        calloc(1, sizeof(decisive_operation));

    operation->length = strlen(curr_operation) + 1;
    operation->operation = (char *)
        calloc(strlen(curr_operation) + 1, sizeof(char));

    strcpy(operation->operation, curr_operation);

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
    
    op_blocks->block_array[++op_blocks->last_index] = block;

    return op_blocks->last_index;
}


char * compare_pair(files_pair * file) 
{
    char * file_name = "tmp";
    char * diff;

    diff =  (char *) 
        calloc(10 + strlen(file->file1) + strlen(file->file2) + strlen(file_name),(sizeof(char)));
            
    strcat(diff, "diff ");
    strcat(diff, file->file1);
    strcat(diff, " ");
    strcat(diff, file->file2);
    strcat(diff, " > ");
    strcat(diff, file_name);

    system(diff);
    free(diff);


    return file_name;
}




void compare_pairs(operation_block_array * op_blocks, files_pair_array * files) 
{
 
    char * file_name = "tmp";

    
    
    for (int i = 0; i < files->len; i++) 
    {
        file_name = compare_pair(files->pairs_array[i]);
        prepare_block(file_name, op_blocks);

    }

}

int number_of_operations(operation_block_array * arr, int index)
{
    return arr->block_array[index]->number_of_decisive_blocks;
}


void remove_block(operation_block_array * arr, int index)
{
    operation_block * block = arr->block_array[index];
    
    for (int i = 0; i < block->number_of_decisive_blocks; i++) 
    {
        free(block->decisive_operations_array[i]->operation);
    }
    free(block->decisive_operations_array);
    free(block);


    arr->block_array[index] = NULL;
    for (int i = index+1; i < arr->size; i++) 
    {
        arr->block_array[i-1] = arr->block_array[i];
    }

    arr->block_array[arr->size-1] = NULL; 
    arr->last_index--;
}

void remove_operation(operation_block_array * arr, int operation_index, int block_index)
{
    operation_block * block = arr->block_array[block_index];
    decisive_operation * op =  block->decisive_operations_array[operation_index];
    
    free(op->operation);
    free(op);

    block->decisive_operations_array[operation_index] = NULL;

    for (int i = operation_index+1; i <= block->number_of_decisive_blocks; i++) 
    {
        block->decisive_operations_array[i-1] = block->decisive_operations_array[i];
    }

    block->decisive_operations_array[block->number_of_decisive_blocks] = NULL;
    block->number_of_decisive_blocks--;
}





