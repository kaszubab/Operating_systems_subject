#include "lib.h"


int depth_global = INT_MAX-1;
int mode_global = 0;
int sgn_global = 0;
int days_global = -1;



int main(int argc, char **argv)
{
    if (argc < 2){
        printf("Please specify directory to be searched\n");
        exit(1);
    }

    if (argc == 2)
    {
            printf("Searching with stat: \n\n");
            search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
    }

    else if (argc == 4)
    {
  
        if(strcmp(argv[2], "-maxdepth") == 0)
        {
            depth_global = atoi(argv[3]);
            
            printf("Searching with stat: \n\n");
            search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
        }
        
        else if(strcmp(argv[2], "-atime") == 0)
        {
            mode_global = 1;

            if ((argv[3][0]) == 43)
            {

                char *ptr = (char *) (&argv[3][1]);
                days_global = atoi(ptr);
                sgn_global = 1;

                printf("Searching with stat: \n\n");
                search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
            }

            else if(argv[3][0] == 45)
            {
                char *ptr = (char *) (&argv[3][1]);
                days_global = atoi(ptr);
                sgn_global = -1;

                printf("Searching with stat: \n\n");
                search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
            }
            
            else
            {
                days_global = atoi(argv[3]);
                sgn_global = 0;

                printf("Searching with stat: \n\n");
                search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
            }
            
        }

        else if(strcmp(argv[2], "-mtime") == 0)
        {
            mode_global = 2;

            if (argv[3][0] == 43)
            {
                char *ptr = (char *) (&argv[3][1]);
                days_global = atoi(ptr);
                sgn_global = 1;

                printf("Searching with stat: \n\n");
                search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
            }

            else if(argv[3][0] == 45) 
            {
                char *ptr = (char *) (&argv[3][1]);
                days_global = atoi(ptr);
                sgn_global = -1;

                printf("Searching with stat: \n\n");
                search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
            }

            else
            {
                days_global = atoi(argv[3]);
                sgn_global = 0;

                printf("Searching with stat: \n\n");
                search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
            }
            
        }

        else
        {
            printf("There is no such option\n");
            return 1;
        }
        
    
    }

    else if (argc == 6)
    {
        if(strcmp(argv[2], "-maxdepth") == 0)
        {
            depth_global = atoi(argv[3]);
            
            if(strcmp(argv[4], "-atime") == 0)
            {
                mode_global = 1;
 
                if (argv[5][0] == 43)
                {
                    char *ptr = (char *) (&argv[5][1]);
                    days_global = atoi(ptr);
                    sgn_global = 1;

                    printf("Searching with stat: \n\n");
                    search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
                }

                else if(argv[5][0] == 45)
                {
                    char *ptr = (char *) (&argv[5][1]);
                    days_global = atoi(ptr);
                    sgn_global = -1;

                    printf("Searching with stat: \n\n");
                    search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
                }

                else
                {
                    days_global = atoi(argv[5]);
                    sgn_global = 0;

                    printf("Searching with stat: \n\n");
                    search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
                }
                
            }
            else if(strcmp(argv[4], "-mtime") == 0){
                mode_global = 2;

                if (argv[5][0] == 43)
                { 
                    char *ptr = (char *) (&argv[5][1]);
                    days_global = atoi(ptr);
                    sgn_global = 1;

                    printf("Searching with stat: \n\n");
                    search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
                }

                else if(argv[5][0] == 45)
                {
                    char *ptr = (char *) (&argv[5][1]);
                    days_global = atoi(ptr);
                    sgn_global = -1;

                    printf("Searching with stat: \n\n");
                    search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
                }

                else
                {
                    days_global = atoi(argv[5]);
                    sgn_global = 0;

                    printf("Searching with stat: \n\n");
                    search_by_stat(argv[1], depth_global, mode_global, sgn_global, days_global);
                }
                
            }
            else
            {
                printf("There is no such option\n");

                return 1;
            }
            }
        else
        {
            printf("There is no such option\n");
            return 1;
        }
        
    }
    else
    {
        printf("There is no such option\n");
        return 1;
    }
    
    return 0;
}