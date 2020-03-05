#ifndef operating_systems_library
#define operating_systems_library

struct operation_block_array {
    int number_of_decisive_blocks;
    char ** decisive_operations_array;
};

struct operation_block_array * create_table(int size);
void compare_pairs(struct operation_block_array * arr,  char * pair);
int split_block(struct operation_block_array * arr, char * block_of_operations);
int number_of_operations(struct operation_block_array * arr, char ** decisive_operations_array);
void remove_block(struct operation_block_array * arr, int index);
void remove_operation(struct operation_block_array * arr, int operation_index, int block_index);



#endif