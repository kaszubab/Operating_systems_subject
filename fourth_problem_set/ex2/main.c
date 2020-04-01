#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void help_function()
{
    printf("\nUsage :  ./program <ignore|handler|mask|pending> \n");
    printf("ignore - ignores SIGUSR1  signal\n");
    printf("handler - handles SIGUSR1  signal - prints it when received\n");
    printf("mask - masks SIGUSR1  signal\n");
    printf("pending - checks if  SIGUSR1  signal is visible\n");

}

void signal_handler(int sig_no)
{
    printf("Received signal %d \n", sig_no);
}

void check_pending_signals()
{
    sigset_t pending;
    sigpending(&pending);

    if( sigismember(&pending, SIGUSR1) == 1 )
    {
        printf("Signal SIGUSR1 is waiting for action\n");
    }
    else
    {
        printf("Signal SIGUSR1 isn't waiting for action\n");
    }
}

int main(int argc, char ** argv)
{
    if (argc != 2)
    {
        printf("Wrong number of arguments, type --help to get help\n");
        exit(EXIT_FAILURE);
    }

    if(strcmp("--help", argv[1]) == 0)
    {
        help_function();
        return 0;
    }
    else if(strcmp("ignore", argv[1]) == 0)
    {
        signal(SIGUSR1, SIG_IGN);

        printf("Main process\n");

        printf("Sending SIGUSR1 signal\n");
        raise(SIGUSR1);
        printf("After SIGUSR1 signal ignored\n");
        fflush(stdout);
        
        if (fork() == 0)
        {


        printf("Child process\n");

        printf("Sending SIGUSR1 signal\n");
        raise(SIGUSR1);
        printf("After SIGUSR1 signal ignored\n");
        fflush(stdout);
            
        }

        else 
        {
            if(fork() == 0)
            {
                execl("./child", "./child", argv[1], NULL);
            }
        }

    }
    else if(strcmp("handler", argv[1]) == 0)
    {
        signal(SIGUSR1, signal_handler);

        printf("Main process\n");

        printf("Sending SIGUSR1 signal\n");
        raise(SIGUSR1);
        printf("After SIGUSR1 signal handled\n");
        fflush(stdout);
        
        if (fork() == 0)
        {

        printf("Child process\n");

        printf("Sending SIGUSR1 signal\n");
        raise(SIGUSR1);
        printf("After SIGUSR1 signal handled\n");
        fflush(stdout);
            
        }


    }
    else if(strcmp("mask", argv[1]) == 0)
    {
        sigset_t new_mask;
        sigemptyset(&new_mask);
        sigaddset(&new_mask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &new_mask, NULL);

        printf("Main process\n");

        printf("Sending SIGUSR1 signal\n");
        raise(SIGUSR1);
        printf("After SIGUSR1 signal blocked\n");
        fflush(stdout);
        
        if (fork() == 0)
        {

        printf("Child process\n");

        printf("Sending SIGUSR1 signal\n");
        raise(SIGUSR1);
        printf("After SIGUSR1 signal blocked\n");
        fflush(stdout);
            
        }

        else 
        {
            if(fork() == 0)
            {
                execl("./child", "./child", argv[1], NULL);
            }
        }


        
    }
    else if(strcmp("pending", argv[1]) == 0)
    {
        sigset_t new_mask;
        sigemptyset(&new_mask);
        sigaddset(&new_mask, SIGUSR1);
        sigprocmask(SIG_BLOCK, &new_mask, NULL);

        printf("Main process\n");

        printf("Sending SIGUSR1 signal\n");
        raise(SIGUSR1);
        check_pending_signals();
        fflush(stdout);
        
        if (fork() == 0)
        {

        printf("Child process\n");

        check_pending_signals();
        fflush(stdout);
            
        }

        else 
        {
            if(fork() == 0)
            {
                execl("./child", "./child", argv[1], NULL);
            }
        }

        
    }
    else
    {
        printf("Illegal arguments!, type --help to get help\n");
        exit(EXIT_FAILURE);
    }


    

    return 0;
}