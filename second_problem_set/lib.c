#include "lib.h"

int generate_random_strings(char * filename, int string_count, int string_len)
{
    FILE * file = fopen(filename, "w");
    
    int seed;
    time_t t;

    seed = time(&t);
    srand(seed);

    for(int i = 0; i < string_count; i++)
    {
        for(int i = 0; i < string_len; i++)
        {
           int shift = rand()%26;
           int choice = rand()%2;
           if (choice == 0)
           {
               fputc('A'+shift, file);
           }
           else
           {
               fputc('a'+shift, file);
           }           
        }

        fputc('\n', file);
    }

    fclose(file);

    return 1;

}


int sort_strings_in_file_sys(char * filename, int string_count, int string_len)
{
    printf("TO DO");
    return -1;
}


int sort_strings_in_file_lib(char * filename, int string_count, int string_len)
{
    printf("TO DO");
    return -1;
}


int copy_file_sys(char * file1, char * file2, int string_count, int string_len)
{
    char * block;
    block = (char * ) 
        calloc(string_len + 1, sizeof(char));

    int file_desc1;
    file_desc1 = open(file1, O_RDONLY);

    int file_desc2;
    file_desc2 = open(file2, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

    int number_of_bytes;
    int strings_read = 0;

    while( (number_of_bytes = read(file_desc1, block, sizeof(*block))) > 0 )
    {
        write(file_desc2, block, sizeof(*block));
        strings_read++;
    }
    if(strings_read < string_count)
        return -1;

    return 1;
}

int copy_file_lib(char * file1, char * file2, int string_count, int string_len)
{
    FILE * f1 = fopen(file1, "r");
    FILE * f2 = fopen(file2, "w");

    char * block;
    block = (char *)
        calloc(string_len+1, sizeof(char));

    if (f1 && f2)
    {
        int read_chars;

        while( (read_chars = fread(block, sizeof(char), string_len+1, f1)) > 0 )
        {
            fwrite( block, sizeof(char), read_chars, f2);
        }
        return 1;
    }

    return -1;
}
