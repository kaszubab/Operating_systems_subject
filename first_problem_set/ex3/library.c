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

// strtol zwraca long

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
            files->pairs_array[pos++] = make_pair(arguments[i]);
        }
        
    }

    files->len = pos;

    return files;
}


operation_block *create_block(char *tmp_name) {

    FILE *handle = fopen(tmp_name, "r");
    if (!handle) {
        printf("error occurred while fopen tmp file: %s", tmp_name);
        return NULL;
    }

    //TODO: check if memory is allocated
    operation_block *diff_block = (operation_block *) calloc(1, sizeof(operation_block));
    diff_block->size = 0;
    diff_block->operations = NULL;

    char *line_buffer = NULL;
    size_t line_buffer_size = 0;
    char *operation = (char *) malloc(0);
    size_t operation_allocated_size = 0; // total allocated size of operation
    size_t operation_len = 0; // size of actual string in operation (including null char)

    while (getline(&line_buffer, &line_buffer_size, handle) >= 0) {
        if (isdigit(line_buffer[0])) {
            if (operation_allocated_size == 0) {
                operation_allocated_size = strlen(line_buffer) + 1;

                operation = (char *) realloc(operation, operation_allocated_size * sizeof(char));
            } else {
                if (diff_block->operations == NULL) {
                    diff_block->operations = (char **) calloc(1, sizeof(char *));
                } else {
                    diff_block->operations = (char **) realloc(diff_block->operations,
                                                               (diff_block->size + 1) * sizeof(char *));
                }
                diff_block->operations[diff_block->size] = (char *) malloc(operation_len * sizeof(char));
                strcpy(diff_block->operations[diff_block->size], operation);
                for (int i = 0; i < operation_len; i++) {
                    operation[i] = '\0';
                }
                diff_block->size++;
            }
            size_t line_buffer_len = strlen(line_buffer);
            if (line_buffer_len > operation_allocated_size) {
                operation = realloc(operation, (line_buffer_len + 1) * sizeof(char));
                operation_allocated_size = line_buffer_len + 1;
            }
            operation_len = line_buffer_len + 1;
            strcpy(operation, line_buffer);
        } else { // continuation of old block
            size_t line_buffer_len = strlen(line_buffer);
            if (operation_len + line_buffer_len > operation_allocated_size) {
                operation = (char *) realloc(operation, (operation_allocated_size + line_buffer_len) * sizeof(char));
                operation_allocated_size += line_buffer_len;
            }

            operation_len += line_buffer_len;
            strcat(operation, line_buffer);
        }
    }

    if (operation_len != 0) {
        if (diff_block->operations == NULL) {
            diff_block->operations = (char **) calloc(1, sizeof(char *));
        } else {
            diff_block->operations = (char **) realloc(diff_block->operations, (diff_block->size + 1) * sizeof(char *));
        }
        diff_block->operations[diff_block->size] = (char *) malloc(operation_len * sizeof(char));
        strcpy(diff_block->operations[diff_block->size], operation);
        diff_block->size++;
    }

    free(line_buffer);
    free(operation);

    return diff_block;
}



int prepare_block(char * file_name, operation_block_array * op_blocks) {
    operation_block *block_to_add = create_block(file_name);
    if (block_to_add == NULL) {
        return -1;
    }

    if (op_blocks->last_index == op_blocks->size - 1) {
        printf("can't add more blocks: table size reached");
        return -1;
    }

    char * remove;
    remove =  (char *) 
        calloc(6 + strlen(file_name),(sizeof(char)));
            
    strcpy(remove, "rm -f ");
    strcat(remove, file_name);

    system(remove);
    free(remove);

    op_blocks->block_array[++op_blocks->last_index] = block_to_add;
    return op_blocks->last_index;
}



/*

int prepare_block(char * file_name, operation_block_array * op_blocks)
{
    FILE * file_pointer;
    printf("WSZYSTKO OK");

    file_pointer = fopen(file_name, "r");

    

    if (file_pointer == NULL) 
    {
        printf("File %s can't be openned right now", file_name);
        return -1;
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

    op_blocks->block_array[++op_blocks->last_index] = block;

    return op_blocks->last_index;
}
*/


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




int compare_pairs(operation_block_array * op_blocks, files_pair_array * files) 
{
 
    char * file_name;
    

    for (int i = 0; i < files->len; i++) 
    {
        file_name = compare_pair(files->pairs_array[i]);
        

        if (prepare_block(file_name, op_blocks) == -1) 
        {
 
            return -1;
        }

        


    
    }
    return 0;

}

int number_of_operations(operation_block_array * arr, int index)
{
    return arr->block_array[index]->size;
}


void remove_block(operation_block_array * arr, int index)
{
    operation_block * block = arr->block_array[index];
    
    for (int i = 0; i < block->size; i++) 
    {
        free(block->operations[i]);
    }
    free(block);


    arr->block_array[index] = NULL;
    for (int i = index+1; i < arr->size; i++) 
    {
        arr->block_array[i-1] = arr->block_array[i];
    }

    arr->block_array[arr->size-1] = NULL; 
    arr->last_index--;
    arr->size--;
}

void remove_operation(operation_block_array * arr, int operation_index, int block_index)
{
    operation_block * block = arr->block_array[block_index];
    free(block->operations[operation_index]);
    
 

    block->operations[operation_index] = NULL;

    for (int i = operation_index+1; i <= block->size; i++) 
    {
        block->operations[i-1] = block->operations[i];
    }

    block->size--;
}





