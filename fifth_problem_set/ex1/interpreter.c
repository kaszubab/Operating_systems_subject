#define _GNU_SOURCE

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/wait.h"


void help_function()
{
    printf("\nUsage :  ./program <file_name> \n");
    printf("filename - name of the file containing commands \n");
}


typedef struct command
{
    char ** command;
    int string_num;
} command;

typedef struct command_line
{
    command ** commands;
    int number_of_commands;
} command_line;



void pipe_commands(command_line * line)
{
    int cur_fd[2];
    int new_fd[2];


    if (pipe(cur_fd) == -1)
        {
            printf("Pipe error");
            exit(EXIT_FAILURE);
        }

    pid_t pid = fork();
    if (pid == 0)
        {
            close(cur_fd[0]);
            if(dup2(cur_fd[1], STDOUT_FILENO) == -1)
            {
                printf("Output setting error");
                exit(EXIT_FAILURE);
            }

            execvp(line->commands[0]->command[0], line->commands[0]->command);
        }
    else
        {
            int status;
            
            waitpid(pid, &status, 0);
            if(WEXITSTATUS(status) != 0)
            {
                printf("Child error");
                exit(EXIT_FAILURE);
            }
        }
        



    
    for (int i = 1; i < line->number_of_commands; i++)
    {



        if (pipe(new_fd) == -1)
        {
            printf("Pipe error");
            exit(EXIT_FAILURE);
        }
  
        pid = fork();
        if (pid == 0)
        {
            close(cur_fd[1]);
            close(new_fd[0]);

            if (dup2(cur_fd[0], STDIN_FILENO) == -1)
            {
                printf("Input setting error");
                exit(EXIT_FAILURE);
            }


            if (dup2(new_fd[1], STDOUT_FILENO) == -1)
            {
                printf("Output setting error");
                exit(EXIT_FAILURE);
            }


            close(cur_fd[0]);
            close(new_fd[1]);


            if(execvp(line->commands[i]->command[0], line->commands[i]->command) == -1)
            {
                printf("Command error");
                exit(EXIT_FAILURE);
            }
        }
        

        close(cur_fd[0]);
        close(cur_fd[1]);
        cur_fd[0] = new_fd[0];
        cur_fd[1] = new_fd[1];

    }


    int status;
    waitpid(pid, &status, 0);
    if(WEXITSTATUS(status) != 0)
    {
        printf("Child error");
        exit(EXIT_FAILURE);
    }
     
    
    char line_read[BUFSIZ];
    read(cur_fd[0], line_read, BUFSIZ);
    printf("%s", line_read);


    
}




int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        printf("Wrong number of arguments. Use --help to get help on function");
    }
    else if(strcmp(argv[1], "--help") == 0)
    {
        help_function();
    }
    else
    {
        FILE * input_file = fopen(argv[1], "r");

        if(input_file == NULL)
        {
            printf("File %s couldnt be openned", argv[1]);
            return -1;
        }

        command_line ** command_lines = (command_line **) calloc(0, sizeof(command_line *));
        int command_lines_num = 0;


        command * curr_command;

        char * line = NULL;
        size_t line_size = 0;

        char * token;
        

        while ((getline(&line, &line_size, input_file))>= 0)
        {

            token = strtok(line, " ");
            if (token == NULL)
            {
                continue;
            }

            command_lines_num++;
            command_lines = (command_line **) realloc(command_lines, sizeof(command_line) * command_lines_num);
            command_lines[command_lines_num-1] = (command_line *) calloc(1, sizeof(command_line));
            command_lines[command_lines_num-1]->number_of_commands = 1;
        
            curr_command = (command *) calloc(1, sizeof(command));
            curr_command->command = (char **) calloc(1, sizeof(char *));
            curr_command->command[0] = (char *) calloc(strlen(token), sizeof(char *));
            strcpy(curr_command->command[0], token);
            curr_command->string_num = 1;

            

            while ((token = strtok(NULL, " \n")) != NULL && strcmp(token, "|") != 0)
            {

                curr_command->string_num++;
                curr_command->command = (char **) realloc(curr_command->command, sizeof(char *) * curr_command->string_num);
                curr_command->command[curr_command->string_num-1] = (char *) calloc(strlen(token), sizeof(char *));
                strcpy(curr_command->command[curr_command->string_num-1], token);
            }

            curr_command->string_num++;
            curr_command->command = (char **) realloc(curr_command->command, sizeof(char *) * curr_command->string_num);
            curr_command->command[curr_command->string_num-1] = NULL;



            command_lines[command_lines_num-1]->commands = (command **) calloc(1, sizeof(command *));
            command_lines[command_lines_num-1]->commands[0] = curr_command;



            while ((token = strtok(NULL, " ")) != NULL )
            {

                curr_command = (command *) calloc(1, sizeof(command));
                curr_command->command = (char **) calloc(1, sizeof(char *));
                curr_command->command[0] = (char *) calloc(strlen(token), sizeof(char *));
                strcpy(curr_command->command[0], token);
                curr_command->string_num = 1;

                while((token = strtok(NULL, " \n")) != NULL && strcmp(token, "|") != 0)
                {
                    
                    curr_command->string_num++;
                    curr_command->command =  realloc(curr_command->command, sizeof(char *) * curr_command->string_num);
                    curr_command->command[curr_command->string_num-1] = (char *) calloc(strlen(token), sizeof(char *));
                    strcpy(curr_command->command[curr_command->string_num-1], token);
                }

                curr_command->string_num++;
                curr_command->command =  realloc(curr_command->command, sizeof(char *) * curr_command->string_num);
                curr_command->command[curr_command->string_num-1] = NULL;

                command_lines[command_lines_num-1]->number_of_commands++;
                command_lines[command_lines_num-1]->commands = (command **) realloc(command_lines[command_lines_num-1]->commands, sizeof(command *) * command_lines[command_lines_num-1]->number_of_commands);
                command_lines[command_lines_num-1]->commands[command_lines[command_lines_num-1]->number_of_commands-1] = curr_command;
    
            }
            

        
        }

        fclose(input_file);
        for ( int i = 0; i < command_lines_num; i++)
        {
            pipe_commands(command_lines[i]);
        }
 
    }
    

    
    return 0;
}