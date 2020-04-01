#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void help_function()
{
    printf("\nUsage :  ./program <division|queue|kill> \n");
    printf("division - division signal\n");

    printf("queue - signal queue\n");
    printf("kill - waiting for kill signal\n");

}

void signal_handler(int sig, siginfo_t * info, void * uncontext)
{
    printf("Received signal %d from user %d \n", info->si_signo, info->si_uid);
    if (info->si_signo == SIGFPE)
    {
        if (info->si_code == FPE_INTDIV)
        {
            printf("Signal code %d that means you cannot divide by zero\n", info->si_code);
            printf("Addres of the fault %p \n", info->si_addr);
            fflush(stdout);
            exit(EXIT_SUCCESS);
        }

    }
    if(info->si_code == SI_QUEUE)
    {

        char *msg = (char *) info->si_value.sival_ptr;

        printf("I received \"%s\" message\n", msg);
        
        free(msg);
        
    }
    if(info->si_code == SI_USER)
    {
        printf("I received signal from kill\n");
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

    struct sigaction action;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = signal_handler;

    if (sigaction(SIGFPE, &action, NULL) == -1)
    {
        printf("Error \n");
    };


    if (sigaction(SIGUSR1, &action, NULL) == -1)
    {
        printf("Error \n");
    };

    if (strcmp(argv[1], "queue") == 0)
    {
        union sigval val;
        char *message = (char *) calloc(19, sizeof(char));
        strcpy(message, "Message from queue");
        val.sival_ptr = (void *) message;

        if(sigqueue(getpid(), SIGUSR1, val) != 0)
        {
            perror("Can't send signal");
            return 1;
        };
    }


    else if (strcmp(argv[1], "division") == 0)
    {
        int a  = 5 - 5;
        int x  = 5/a;
        printf("%d \n", x);
    }

    else if (strcmp(argv[1], "kill") == 0)
    {
        pause();
    }

    else
    {
        printf("Illegal argument %s", argv[1]);
        exit(EXIT_FAILURE);
    }
    
    return 0;
}