#include "lib.h"

int partition_sys(int desc, int left, int right, int line_size);
void quicksort_sys(int desc, int left, int right, int line_size);
int partition_lib(FILE * file, int left, int right, int line_size);
void quicksort_lib(FILE * file, int left, int right, int line_size);

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
    int file_desc1;
    file_desc1 = open(filename, O_RDWR);

    if(file_desc1 == -1)
    {
        perror("Cannot open source file");
        exit(1);
    }

    quicksort_sys(file_desc1, 0, string_count-1, string_len +1);
    return 1;
}

int partition_sys(int desc, int left, int right, int line_size)
{
    char * pivot;
    char * curr_line;

    pivot = (char *)
        calloc(line_size, sizeof(char));

    curr_line = (char *)
        calloc(line_size, sizeof(char));

    lseek(desc, line_size * right, SEEK_SET);
    read(desc, pivot, line_size);



    int i = left -1;
    int j;

    for(j = left; j < right; j++)
    {
        lseek(desc, line_size * j, SEEK_SET);
        read(desc, curr_line, line_size);

        if(strcmp(pivot, curr_line) > 0)
        {
            i++;
            lseek(desc, line_size * i, SEEK_SET);
            read(desc, pivot, line_size);

            lseek(desc, line_size * i, SEEK_SET);
            write(desc, curr_line, line_size);

            
            lseek(desc, line_size * j, SEEK_SET);
            write(desc, pivot, line_size);

            lseek(desc, line_size * right, SEEK_SET);
            read(desc, pivot, line_size);

        }
    }

    lseek(desc, line_size * (i + 1), SEEK_SET);
    read(desc, curr_line, line_size);

    lseek(desc, line_size * right, SEEK_SET);
    write(desc, curr_line, line_size);

    lseek(desc, line_size * (i + 1), SEEK_SET);
    write(desc, pivot, line_size);

    return i+1;
}

void quicksort_sys(int desc, int left, int right, int line_size)
{

    if (left < right)
    {
        int pivot = partition_sys(desc, left,right, line_size);
        quicksort_sys(desc, left, pivot-1, line_size);
        quicksort_sys(desc, pivot+1, right, line_size);
    }
}



int sort_strings_in_file_lib(char * filename, int string_count, int string_len)
{
    FILE * file = fopen(filename, "r+");
    quicksort_lib(file, 0, string_count-1, string_len+1);
    return 1;
}


int partition_lib(FILE * file, int left, int right, int line_size)
{

    char * pivot;
    char * curr_line;

    pivot = (char *)
        calloc(line_size, sizeof(char));

    curr_line = (char *)
        calloc(line_size, sizeof(char));


    fseek(file, line_size * right, 0);
    fread(pivot, sizeof(char), line_size, file);

    int i = left -1;
    int j;

    
    for(j = left; j < right; j++)
    {
        fseek(file, line_size * j, 0);
        fread(curr_line, sizeof(char), line_size, file);
        
        if(strcmp(pivot, curr_line) > 0)
        {
            i++;
            fseek(file, line_size * i, 0);
            fread(pivot, sizeof(char), line_size, file);

            fseek(file, line_size * i, 0);
            fwrite(curr_line, sizeof(char),line_size, file);

            
            fseek(file, line_size * j, 0);
            fwrite(pivot, sizeof(char),line_size, file);
         
            fseek(file, line_size * right, 0);
            fread(pivot, sizeof(char), line_size, file);
        }
    }
    

    fseek(file, line_size * (i+1), 0);
    fread(curr_line, sizeof(char), line_size, file);

    fseek(file, line_size * right, 0);
    fwrite(curr_line, sizeof(char), line_size, file);

    fseek(file, line_size * (i+1), 0);
    fwrite(pivot, sizeof(char), line_size, file);
    
    return i+1;
}

void quicksort_lib(FILE * file, int left, int right, int line_size)
{
    if (left < right)
    {
        int pivot = partition_lib(file, left,right,line_size);
        quicksort_lib(file, left, pivot-1, line_size);
        quicksort_lib(file, pivot+1, right, line_size);
    }
}






int copy_file_sys(char * file1, char * file2, int string_count, int string_len)
{
    char * block;
    block = (char * ) 
        calloc(string_len + 1, sizeof(char));

    int file_desc1;
    file_desc1 = open(file1, O_RDONLY);

    if(file_desc1 == -1)
    {
        perror("Cannot open source file");
        exit(1);
    }

    int file_desc2;
    file_desc2 = open(file2, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

    if(file_desc2 == -1)
    {
        perror("Cannot open or create result file");
        exit(1);
    }

    int number_of_bytes;
    int strings_read = 0;

    while( (number_of_bytes = read(file_desc1, block, string_len+1)) > 0 )
    {
        write(file_desc2, block, number_of_bytes);
        strings_read++;
    }

    if(strings_read < string_count)
        return -1;

    close(file_desc1);
    close(file_desc2);
    free(block);

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
