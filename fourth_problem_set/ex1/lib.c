#include "lib.h"

char * get_file_type(unsigned char file_type)
{
    switch (file_type)
    {
    case 6: //DT_BLK
        return "block device";
        break;
    
    case 2: //DT_CHR
        return "character device";
        break;
    
    case 4: // DT_DIR
        return "directory";
        break;
    
    case 1: //DT_FIFO
        return "named pipe (FIFO)";
        break;
    
    case 10: //DT_LNK
        return "symbolic link";
        break;

    case 8: //DT_REG
        return "regular file";
        break;
    
    case 12: //DT_SOCK
        return "socket";
        break;
    
    default:
        return "unknown";
    }

}

int check_time(struct tm *input_time, int sgn, int days)
{
    int checked = 0;

    checked = input_time->tm_mday + 31*input_time->tm_mon + 365*input_time->tm_year;
   
    time_t current_time_val;
    struct tm *current_time;
    current_time_val = time(NULL);
    current_time = localtime(&current_time_val);


    int curr = 0;
    curr = current_time->tm_mday + 31*current_time->tm_mon + 365*current_time->tm_year;
        
    if (sgn == 0){
        if (curr - checked == days) return 1;
        else return 0;
    }
    else if(sgn == 1)
    {
        if(curr - checked > days) return 1;
        else return 0;
    }
     else if(sgn == -1)
    {
        if(curr - checked < days) return 1;
        else return 0;
    }
    return 0;
} 

void print_stat_info(char * absolute_path, struct stat * stats, unsigned char file_type, char * access_time, char * modification_time)
{
    printf("File path : %s \n", absolute_path);
    printf("Number of links: %ld \n", stats->st_nlink);
    printf("Type of file: %s \n", get_file_type(file_type));
    printf("Size in bytes: %ld \n", stats->st_size);
    printf("Last access time: %s \n", access_time);
    printf("Last modification time: %s \n \n", modification_time);

}


void search_by_stat(char * dir_path, int depth, int mode, int sgn, int days)
{

    if (depth < 0)
    {
        return;
    }

    if (dir_path == NULL)
    {
        printf("%s - invalid directory name", dir_path);
        exit(1);
    }

    while(1)
    {

        DIR * directory = opendir(dir_path);

        if (directory == NULL)
        {
            printf("directory %s could not be found", dir_path);
            exit(1);
        }

        struct dirent *file_pointer;
        struct tm *mod_time;
        struct tm *acc_time;

        struct stat * file_stats;
        file_stats = (struct stat *) calloc(1, sizeof(struct stat));
        
        char * abs_path;
        char * next_path;

        abs_path = (char *) calloc(200, sizeof(char));
        next_path = (char *) calloc(200, sizeof(char));

        while( (file_pointer = readdir(directory)) != NULL)
        {
            if (strcmp(file_pointer->d_name, ".") == 0 || strcmp(file_pointer->d_name, "..") == 0)
            continue;

            strcpy(next_path, dir_path);
            strcat(next_path, "/");
            strcat(next_path, file_pointer->d_name);

            realpath(next_path, abs_path);

            if( lstat(next_path, file_stats) < 0)
            {
                perror("lstat error");
                exit(1);
            }   

            mod_time = localtime(&file_stats->st_mtime);

            char * modification_time = (char *)
                calloc(40, sizeof(char));

            if (strftime(modification_time, 40, "%d.%m.%Y", mod_time) == 0){
                printf("Error during convering date to string!\n");
            }
            
            char *access_time = (char *) calloc(40, sizeof(char));

            acc_time = localtime(&file_stats->st_atime);

            if (strftime(access_time, 40, "%d.%m.%Y", acc_time) == 0){
                printf("Error during convering date to string!\n");
            }


            switch (mode)
            {
            case 0:
                print_stat_info(abs_path, file_stats, file_pointer->d_type, access_time, modification_time);
                break;
            
            case 1:
                if ( check_time(acc_time, sgn, days))
                {
                    print_stat_info(abs_path, file_stats, file_pointer->d_type, access_time, modification_time);
                }
                break;

            case 2:
                if ( check_time(mod_time, sgn, days))
                {
                    print_stat_info(abs_path, file_stats, file_pointer->d_type, access_time, modification_time);
                }
                break;

            default:
                break;
            }

        

            free(access_time);
            free(modification_time);
            sleep(1);

        }



        free(abs_path);
        free(next_path);

        free(file_pointer);
        closedir(directory);
    }

}
