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


void help_function()
{
    printf("\nUsage :  ./program <catcher_id> <sig_num>  <mode> \n");
    printf("catcher_id - pid of catcher\n");
    printf("sig_num - number of signals to send\n");
    printf("mode - KILL|SIGQUEUE|SIGRT\n");
    printf("mode - KILL - simple kill signals sender");
    printf("mode - SIGQUEUE - receive additional information\n");
    printf("mode - SIGRT - send real time signals with kill\n");


}

int received_signals = 0;
int signal_count;
sigset_t mask;


void signal_handler(int sig, siginfo_t * info, void * uncontext)
{

    received_signals++;
}



void kill_SIGUSR2_handler(int sig, siginfo_t * info, void * context)
{
    printf("SENDER : SENT %d signals. \n", signal_count);
    printf("SENDER : RECEIVED %d signals. Goodbye \n", received_signals);
    exit(EXIT_SUCCESS);
}




void queue_SIGUSR2_handler(int sig, siginfo_t * info, void * context)
{

    printf("SENDER : SENT %d signals. \n", signal_count);


    int msg = (int) info->si_value.sival_int;

    printf("CATCHER : SENT %d signals\n", msg);
    printf("SENDER : RECEIVED %d signals. Goodbye \n", received_signals);
    // free(msg);
    exit(EXIT_SUCCESS);



}

void sigRT_SIGUSR2_handler(int sig, siginfo_t *info, void *context)
{

    printf("SENDER : SENT %d signals. \n", signal_count);
    printf("SENDER : RECEIVED %d signals. Goodbye \n", received_signals);
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




void send_signals_kill(int signal_number, int signal_number2, pid_t catcher)
{
    for (int i = 0; i < signal_count; i++)
    {

        fflush(stdout);
        if (kill(catcher, signal_number) != 0)
        {
            printf("Error \n");
            fflush(stdout);
        }
    }

    if (kill(catcher, signal_number2) != 0)
        {
            printf("Error \n");
            fflush(stdout);
        }
}

void send_queue(pid_t catcher)
{
    union sigval val;

    for(int i = 0; i < signal_count; i++)
    {
        val.sival_int = i;
        if(sigqueue(catcher, SIGUSR1, val) != 0){
            perror("Error");
            exit(EXIT_FAILURE);
        }
    }
    
    val.sival_int = signal_count;

    if(sigqueue(catcher, SIGUSR2, val) != 0){
            perror("Error");
            exit(EXIT_FAILURE);
    }

}


int main(int argc, char ** argv)
{
    if (argc != 4)
    {
        printf("Wrong number of arguments %d", argc);
        help_function();
        exit(EXIT_FAILURE);
    }



    pid_t target_proc_id = atoi(argv[1]);

    signal_count = atoi(argv[2]);

    char * signal_mode = argv[3];

    if (strcmp(signal_mode, "KILL") == 0)
    {

        set_mask(KILL);
        handle_initial_signals(KILL);
        send_signals_kill(SIGUSR1,SIGUSR2, target_proc_id);

    }
    else if(strcmp(signal_mode, "SIGQUEUE") == 0)
    {
        set_mask(QUEUE);
        handle_initial_signals(QUEUE);
        send_queue(target_proc_id);
    }
    else if(strcmp(signal_mode, "SIGRT") == 0)
    {
        set_mask(SIGRT);
        handle_initial_signals(SIGRT);
        send_signals_kill(SIGRTMIN,SIGRTMIN + 1, target_proc_id);
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