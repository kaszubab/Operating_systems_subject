#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>


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
        printf("Illegal arguments!, type --help to get help\n");
        exit(EXIT_FAILURE);
    }



    if(strcmp("ignore", argv[1]) == 0)
    {

        printf("EXEC process\n");

        printf("Sending SIGUSR1 signal\n");
        raise(SIGUSR1);
        printf("After SIGUSR1 signal ignored\n");
        fflush(stdout);
        
    }


    if(strcmp("mask", argv[1]) == 0)
    {
        printf("EXEC process\n");

        printf("Sending SIGUSR1 signal\n");
        raise(SIGUSR1);
        printf("After SIGUSR1 signal blocked\n");
        fflush(stdout);
   
    }


    if(strcmp("pending", argv[1]) == 0)
    {
        printf("EXEC process\n");

        check_pending_signals();
        fflush(stdout);
   
    }


    

    return 0;
}