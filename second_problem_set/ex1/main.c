#include "lib.h"
#include <fcntl.h>
#include <time.h>
#include <sys/times.h>

void print_help(void) 
{
    printf("List of commands:\n");
    printf("generate <filename> <records_number> <record_length> - generate file with <records_number> of records - each consisting of <record_length> random characters\n");
    printf("sort  <filename> <records_number> <record_length> [sys/lib] - sort lines in <filename> file in lexicographical order\n");
    printf("copy <file1> <file2> <records_number> <record_length> [sys/lib] - copy <record_number> lines of <record_length> from file1 int file 2 using either system functions or library ones\n");
}

char *available_commands[3] = {"generate", "sort", "copy"};

int check_command(char* com)
{
    for(int i = 0;i<3;i++)
    {
        if(strcmp(com, available_commands[i]) == 0)
        {
            return i+1;
        }
    }
    return 0;
}

double get_time(clock_t start, clock_t end)
{
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

void write_time(FILE * result_file,struct tms *start,struct tms *end){
    fprintf(result_file,"\tUSER_TIME: %fl\n", get_time(start->tms_utime,end->tms_utime));
    fprintf(result_file,"\tSYSTEM_TIME: %fl\n\n", get_time(start->tms_stime,end->tms_stime));
}



int main(int argc, char **argv) 
{

    FILE * result_file;

    if (argc == 2 && strcmp(argv[1], "--help") == 0) 
    {
        print_help();
        return 0;
    }

    if (argc < 2 || check_command(argv[1]) == 0) 
    {
        printf("error parsing arguments -> use --help to check the list of available commands \n ");
        exit(1);
    }

    struct tms * start = (struct tms *) calloc (1,sizeof(struct tms));
    struct tms * end = (struct tms *) calloc (1,sizeof(struct tms));

    result_file = fopen("results.txt", "a");
    if (result_file == NULL)
    {
        perror("Result file couldnt be openned");
        exit(1);
    }

        
    int command = check_command(argv[1]);
    
    if (command == 1) 
    {
        if (argc < 5) 
        {
            printf("error parsing arguments -> use --help to check the list of available commands \n ");
            fclose(result_file);
            exit(1);
        }

        char * file = argv[2];
        int lines_num = (int) strtol(argv[3], NULL, 10); 
        int line_size = (int) strtol(argv[4], NULL, 10); 

        generate_random_strings(file, lines_num, line_size);
    }
    else if (command == 2) 
    {

        if (argc < 6) 
        {
            printf("error parsing arguments -> use --help to check the list of available commands \n ");
            fclose(result_file);
            exit(1);
        }

        char * file = argv[2];
        int lines_num = (int) strtol(argv[3], NULL, 10); 
        int line_size = (int) strtol(argv[4], NULL, 10); 

        char * sort_mode = argv[5];

        if (strcmp(sort_mode, "lib"))
        {
            times(start);
            sort_strings_in_file_lib(file, lines_num, line_size);
            times(end);
            fprintf(result_file, "Sort in mode lib file having %d lines %d bytes each\n", lines_num, line_size);
            write_time(result_file, start, end);
        }
        else
        {
            times(start);
            sort_strings_in_file_sys(file, lines_num, line_size);
            times(end);
            fprintf(result_file, "Sort in mode sys file having %d lines %d bytes each\n", lines_num, line_size);
            write_time(result_file, start, end);
        }
        
    }
    else if (command == 3) 
    {

        if (argc < 7) 
        {
            printf("error parsing arguments -> use --help to check the list of available commands \n ");
            fclose(result_file);
            exit(1);
        }

        char * file1 = argv[2];
        char * file2 = argv[3];

        int lines_num = (int) strtol(argv[4], NULL, 10); 
        int line_size = (int) strtol(argv[5], NULL, 10); 

        printf(" %d %d \n", lines_num, line_size);

        char * copy_mode = argv[6];

        if (strcmp(copy_mode, "lib"))
        {
            times(start);
            copy_file_lib(file1, file2, lines_num, line_size);
            times(end);
            fprintf(result_file, "Copy in mode lib file having %d lines %d bytes each\n", lines_num, line_size);
            write_time(result_file,start, end);
        }
        else
        {
            times(start);
            copy_file_sys(file1, file2, lines_num, line_size);
            times(end);
            fprintf(result_file, "Copy in mode sys file having %d lines %d bytes each\n", lines_num, line_size);
            write_time(result_file, start, end);
        }
    }


    fclose(result_file);
    return 0;
}



