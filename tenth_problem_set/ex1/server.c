#define _GNU_SOURCE

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "pthread.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include "shared.h"
#include "sys/epoll.h"



char * server_port;
char * client_nicknames[MAX_CLIENTS];
int last_client = 0;
int last_name = 0;

int remote_socket_fd;
int clients[MAX_CLIENTS];
int responding[MAX_CLIENTS];
int waiting_for_game_fd = -1;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int epoll_desc;


struct epoll_mess
{
    int fd;
    int message_type;
    int other_fd;
};



void help_function()
{
    printf("\nUsage :  ./server <TCP/UDP port number> <UNIX_socket address> \n");
}

void error (char *msg)
{
    printf("%s \n", msg);
    exit(EXIT_FAILURE);
}

int get_index_by_fd(int fd)
{
    for(int i = 0 ; i < last_client; i++)
    {
        if(clients[i] == fd)
            return i;
    }
    return -1;
}

void delete_client(int client_fd)
{
    printf("Deleting %d \n",client_fd);
    int ind = get_index_by_fd(client_fd);
    clients[ind] = -1;
    client_nicknames[ind] = NULL;
    
    if (waiting_for_game_fd == client_fd)
                        waiting_for_game_fd = -1; 
    if (epoll_ctl(epoll_desc, EPOLL_CTL_DEL, client_fd, NULL) == -1)
    {
        error("Epoll ctl");
    }
}




void * ping_thread(void * arg)
{


    while(TRUE)
    {
        sleep(10);
        pthread_mutex_lock(&mutex);
        for(int i = 0; i < last_client; i++)
        {
            if ( clients[i] != -1)
            {
                if ( responding[i] == 1)
                {
                    net_message response;
                    response.message_type = PING;

                    if(send(clients[i], &response, sizeof(response) ,0) == -1)
                    {
                        error("send");
                    }
                }
                else
                {
                    delete_client(clients[i]);
                }
                
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}


int register_name(char * nickname)
{
    for(int i = 0; i < last_name; i++)
    {
        if (client_nicknames[i] != NULL && strcmp(client_nicknames[i], nickname) == 0)
        {
            return -1;
        }
    }
    client_nicknames[last_name] = (char *) calloc(strlen(nickname), sizeof(char));
    strcpy(client_nicknames[last_name++], nickname);
    return 0;
}



void * game_thread(void * arg)
{

    int MAX_EVENTS = 10;
    struct epoll_event events [MAX_EVENTS];

    while(1)
    {
        // printf("Working \n");
        int event_count = epoll_wait(epoll_desc, events, MAX_EVENTS, 1000);

        for (int i = 0; i < event_count; i++)
        {
            printf("Processing \n");
            struct epoll_event event = events[i];
            struct epoll_mess * message = event.data.ptr;
            if (message->message_type == WAITING_FOR_NICKNAME)
            {
                net_message initial;
                read(message->fd, &initial, sizeof(net_message));
                char nickname[MAX_NICKNAME_LENGTH];

                if (initial.message_type == WAITING_FOR_NICKNAME)
                {
                    strcpy(nickname,initial.message_text);
                }
                else if (initial.message_type == QUIT)
                {     
                    printf("Closing connection %d \n", message->fd);     
                    pthread_mutex_lock(&mutex);          
                    delete_client(message->fd);
                    pthread_mutex_unlock(&mutex);
                    continue;
                }
                if (initial.message_type == PING)
                {
                    int ind = get_index_by_fd(message->fd);
                    responding[ind] = 1;
                }
                
                if(register_name(nickname) != 0)
                {
                    net_message response;
                    response.message_type = NICKNAME_OCCUPIED;

                    if(send(message->fd, &response, sizeof(response) ,0) == -1)
                    {
                        error("send");
                    }

                    continue;
                }
                else 
                {
                    net_message response;
                    response.message_type = NICKNAME_ACCEPTED;

                    if(send(message->fd, &response, sizeof(response) ,0) == -1)
                    {
                        error("send");
                    }
                }

                if(waiting_for_game_fd == -1)
                {
                    waiting_for_game_fd = message->fd;

                    net_message response;
                    response.message_type = WAITING_FOR_OTHER_PLAYER;

                    if(send(message->fd, &response, sizeof(response) ,0) == -1)
                    {
                        error("send");
                    }
                }
                else
                {
                    net_message start_game;
                    start_game.message_type = PLAYING;
                    
                    for (int i = 0; i < 9; i++)
                        start_game.game_arr[i] = EMPTY;


                    start_game.marker = X_marker;

                    struct epoll_event event;
                    event.events = EPOLLIN;
                    struct epoll_mess * data = (struct epoll_mess *) calloc(1, sizeof(struct epoll_mess));
                    data->fd = message->fd;
                    data->other_fd = waiting_for_game_fd;
                    data->message_type = PLAYING;
                    event.data.ptr = data;


                    if (epoll_ctl(epoll_desc, EPOLL_CTL_MOD, message->fd, &event) == -1)
                    {
                        error("Epoll ctl");
                    }


                    struct epoll_event second_event;
                    second_event.events = EPOLLIN;
                    struct epoll_mess * data2 = (struct epoll_mess *) calloc(1, sizeof(struct epoll_mess));
                    data2->fd = waiting_for_game_fd;
                    data2->other_fd = message->fd;
                    data2->message_type = PLAYING;
                    second_event.data.ptr = data2;
                    

                    if (epoll_ctl(epoll_desc, EPOLL_CTL_MOD, waiting_for_game_fd, &second_event) == -1)
                    {
                        error("Epoll ctl");
                    }

                    
                    int choice = rand() % 2;
                    printf("choice %d \n", choice);
                    if (choice == 0)
                    {
                        strcpy(start_game.message_text,client_nicknames[get_index_by_fd(waiting_for_game_fd)]);   
                        if(send(message->fd, &start_game, sizeof(start_game) ,0) == -1)
                        {
                            error("send");
                        }
                    
                    }               
                    else 
                    {
                        strcpy(start_game.message_text,client_nicknames[get_index_by_fd(message->fd)]);   
                        if(send(waiting_for_game_fd, &start_game, sizeof(start_game) ,0) == -1)
                        {
                            error("send");
                        }
                    }

                    waiting_for_game_fd = -1;

                }
                


                printf("Hello %s\n", nickname);
                memset(nickname, 0, MAX_NICKNAME_LENGTH * sizeof(char));
            }
            else if (message->message_type == PLAYING)
            {
                printf("%d and %d are playing \n",message ->fd, message->other_fd);

                net_message mess; 
                int bytes_read = recv ( message->fd , &mess, sizeof (mess),0) ;

                if (bytes_read== -1)
                    error ("recv");

                if (mess.message_type == PING)
                {
                    int other_ind = get_index_by_fd(message->other_fd);
                    if ( other_ind == -1) 
                    {
                        net_message kill_messsage;
                        kill_messsage.message_type = QUIT;
                        kill_messsage.marker = LOST;
                        if(send(message->fd, &kill_messsage, sizeof(kill_messsage), 0) == -1)
                        {
                            error("Send kill");
                        } 
                    }
                    else
                    {
                        int ind = get_index_by_fd(message->fd);
                        responding[ind] = 1;
                    }

                    continue;
                    
                }

                if (mess.message_type == QUIT)
                {
                    printf("Closing connection %d  \n", message->fd);              
                    pthread_mutex_lock(&mutex);
                    delete_client(message->fd);
                    pthread_mutex_unlock(&mutex);

                    if (get_index_by_fd(message->other_fd) == -1)
                    {
                        continue;
                    }
                }


                

                if (mess.marker == X_marker)
                    mess.marker = O_marker;
                else if(mess.marker != LOST)
                    mess.marker = X_marker;


                memset(mess.message_text,0, strlen(mess.message_text) * sizeof(char));
                strcpy(mess.message_text, client_nicknames[get_index_by_fd(message->other_fd)]);


                if(send(message->other_fd, &mess, sizeof(mess) ,0) == -1)
                {
                    error("send");
                }


            }

            

            
        }

    }

    

    return NULL;
}




int main(int argc, char ** argv)
{
    

    if (argc != 3 && argc != 2)
     {
        printf("Invalid number of arguments use --help to get help");
        exit(EXIT_FAILURE);
    }

    if (argc == 2)
    {
        if (strcmp(argv[1], "--help") != 0) 
        {
            printf("Illegal argument %s", argv[1]);
            exit(EXIT_FAILURE);
        }

        help_function();
        exit(EXIT_SUCCESS);
        
    }
    
    if (argc == 3)
    {

        srand(time(NULL));  

        server_port = argv[1];
        char* socket_path = argv[2];
        int epoll_fd = epoll_create1(0);

        if(epoll_fd == -1)
        {
            error("epoll_desc");
        }
        
        int local_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
        if ( bind(local_fd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
            error("Unix bind \n");

        if(listen(local_fd, MAX_CLIENTS) != 0)
        {
            error("Local listen error");
        }


        for ( int i = 0; i < MAX_CLIENTS; i++)
        {
            client_nicknames[i] = NULL;
        }

        struct addrinfo hints;
        memset(&hints, 0, sizeof (struct addrinfo));

        hints.ai_family = AF_INET;   
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;  

        struct addrinfo *result;
        int s; 

        if ((s = getaddrinfo (NULL, server_port, &hints, &result)) != 0) {
            error("Getaddrinfo error");
        }

        struct addrinfo * curr_socket;

        for (curr_socket = result; curr_socket != NULL; curr_socket = curr_socket->ai_next)
        {
            remote_socket_fd = socket( curr_socket->ai_family, curr_socket->ai_socktype, curr_socket->ai_protocol);

            if (remote_socket_fd == -1)
                continue;

           if (bind (remote_socket_fd, curr_socket -> ai_addr, curr_socket -> ai_addrlen) == 0) 
                break; 

        }

        if (curr_socket == NULL)
        {
            error("Not able to bind");
        }

        freeaddrinfo(result);

        if(listen(remote_socket_fd, MAX_CLIENTS) != 0)
        {
            error("Listen error");
        }


        epoll_desc = epoll_create1(0);

        if(epoll_desc == -1)
        {
            error("epoll_desc");
        }


        pthread_t game;
        
        if(pthread_create(&game, NULL, game_thread, &epoll_desc) != 0)
        {
            printf("Game creation error");
            exit(EXIT_FAILURE);
        }

        pthread_t ping;
        
        if(pthread_create(&ping, NULL, ping_thread, NULL) != 0)
        {
            printf("Ping creation error");
            exit(EXIT_FAILURE);
        }

 


        struct epoll_event event;
        event.events = EPOLLIN;
        struct epoll_mess * data = (struct epoll_mess *) calloc(1, sizeof(struct epoll_mess));
        data->fd = remote_socket_fd;
        event.data.ptr = data;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, remote_socket_fd, &event) == -1)
        {
            error("Epoll ctl remote");
        }


        

        struct epoll_event event2;
        event2.events = EPOLLIN;
        struct epoll_mess * data2 = (struct epoll_mess *) calloc(1, sizeof(struct epoll_mess));
        data2->fd = local_fd;
        event2.data.ptr = data2;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, local_fd, &event2) == -1)
        {
            error("Epoll ctl local");
        }
        

        int MAX_EVENTS = 10;
        struct epoll_event events [MAX_EVENTS];

        while(1)
        {

            int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 5000);
            printf("Event count %d \n",event_count);

            for (int i = 0; i < event_count; i++)
            {
                printf("Processing \n");
                struct epoll_event event_poll = events[i];
                struct epoll_mess * message = event_poll.data.ptr;


                struct sockaddr_in their_addr; 
                socklen_t their_size = sizeof(struct sockaddr_in);
                
                int client_desc = accept(message->fd,(struct sockaddr *) &their_addr, &their_size); 

                pthread_mutex_lock(&mutex);
                clients[last_client] = client_desc;
                responding[last_client] = 1;
                printf("File descriptor %d \n Welcome %s \n", client_desc, inet_ntoa(their_addr.sin_addr));
                
                struct epoll_event event;
                event.events = EPOLLIN;
                struct epoll_mess * data = (struct epoll_mess *) calloc(1, sizeof(struct epoll_mess));
                data->fd = client_desc;
                data->message_type = WAITING_FOR_NICKNAME;
                event.data.ptr = data;
                

                if (epoll_ctl(epoll_desc, EPOLL_CTL_ADD, client_desc, &event) == -1)
                {
                    error("Epoll ctl");
                }
                
                last_client++;
                pthread_mutex_unlock(&mutex);
            }

        }


        /*

        pthread_t ping;
        
        if(pthread_create(&ping, NULL, ping_thread, NULL) != 0)
        {
            printf("Barber creation error");
            exit(EXIT_FAILURE);
        }
        */

        
    }

    return 0;

}