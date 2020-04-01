#define _XOPEN_SOURCE 500

#define SIGRT 2
#define QUEUE 1
#define KILL 0


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

int received_signals = 0;
sigset_t mask;
pid_t sender_id = -1;


void signal_handler(int sig, siginfo_t * info, void * uncontext)
{
    if (sender_id == -1)
    {
        sender_id = info->si_pid;

    }
    received_signals++;
}



void kill_SIGUSR2_handler(int sig, siginfo_t * info, void * context)
{
    printf("CATCHER : RECEIVED %d signals. Goodbye \n", received_signals);

    for (int i = 0; i < received_signals; i++)
    {
        kill(sender_id, SIGUSR1);
    }

    kill(sender_id, SIGUSR2);
    exit(EXIT_SUCCESS);
}




void queue_SIGUSR2_handler(int sig, siginfo_t * info, void * context){

    
    union sigval val;

    printf("CATCHER : RECEIVED %d signals. Goodbye \n", received_signals);
    
    for (int i = 0 ; i < received_signals; i++){
        val.sival_int = i;
        sigqueue(sender_id, SIGUSR1, val);
    }
    
    val.sival_int = received_signals;

    sigqueue(sender_id, SIGUSR2, val);
    exit(EXIT_SUCCESS);


}

void sigRT_SIGUSR2_handler(int sig, siginfo_t *info, void *context)
{

    printf("CATCHER : RECEIVED %d signals. Goodbye \n", received_signals);

    for (int i = 0; i < received_signals; i++)
    {
        kill(sender_id, SIGRTMIN);
    }

    kill(sender_id, SIGRTMIN+1);
    exit(EXIT_SUCCESS);
}



void handle_initial_signals(int mode)
{

    struct sigaction action;
    action.sa_mask = mask;
    action.sa_flags = SA_SIGINFO;

    struct sigaction action2;
    action2.sa_flags = SA_SIGINFO;
    action2.sa_mask = mask;

        
    switch(mode)
    {
        case KILL:
            action.sa_sigaction = signal_handler;
            if (sigaction(SIGUSR1, &action, NULL) == -1)
            {
                printf("Error \n");
                exit(EXIT_FAILURE);
            };


            action2.sa_sigaction = kill_SIGUSR2_handler;

            if (sigaction(SIGUSR2, &action2, NULL) == -1)
            {
                printf("Error \n");
            };

        break;
        case QUEUE:
            action.sa_sigaction = signal_handler;
            if (sigaction(SIGUSR1, &action, NULL) != 0){
                printf("Error \n");
                exit(EXIT_FAILURE);
            }

            action2.sa_sigaction = queue_SIGUSR2_handler;

            if (sigaction(SIGUSR2, &action2, NULL) == -1)
            {
                printf("Error \n");
            };
        break;

        case SIGRT: 
            action.sa_sigaction = signal_handler;
            if (sigaction(SIGRTMIN, &action, NULL) != 0){
                printf("Error \n");
                exit(EXIT_FAILURE);
            }

            action2.sa_sigaction = sigRT_SIGUSR2_handler;

            if (sigaction(SIGRTMIN + 1, &action2, NULL) == -1)
            {
                printf("Error \n");
                exit(EXIT_FAILURE);
            };

            break;
        default:
            printf("No such mode\n");
            exit(1);
            break;
    }
        
}


void set_mask(int real_time_mode)
{
    sigfillset(&mask);

    if(real_time_mode == SIGRT)
    {
        sigdelset(&mask, SIGRTMIN);
        sigdelset(&mask, SIGRTMIN+1);
    }
    else if(real_time_mode == KILL || real_time_mode == QUEUE)
    {
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGUSR2);

    }

    if (sigprocmask(SIG_BLOCK, &mask, NULL) != 0)
    {
        printf("Error settinf signal mask");
        exit(EXIT_FAILURE);
    }
}



int main(int argc, char ** argv)
{

    printf("My PID %d", getpid());
    fflush(stdout);


    if (strcmp(argv[1], "KILL") == 0)
    {
       
        set_mask(KILL);

        handle_initial_signals(KILL);

    }
    else if(strcmp(argv[1], "SIGQUEUE") == 0)
    {
        set_mask(QUEUE);
        handle_initial_signals(QUEUE);
    }
    else if(strcmp(argv[1], "SIGRT") == 0)
    {
        set_mask(SIGRT);
        handle_initial_signals(SIGRT);
    }
    else
    {
        printf("Illegal mode value %s", argv[3]);
        exit(EXIT_FAILURE);
    }

    while(1)
    {
        pause();
    }
    

    return 0;
}