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
struct sockaddr* clients[MAX_CLIENTS];
int current_games[MAX_CLIENTS];
int responding[MAX_CLIENTS];
int waiting_for_game_ind = -1;

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



int get_index_by_address(struct sockaddr * addr)
{
    for(int i = 0 ; i < last_client; i++)
    {        
        if(clients[i] != NULL && memcmp(clients[i], addr, sizeof(*addr)) == 0)
            return i;
    }
    return -1;
}

void delete_client(int client_ind)
{
    printf("Deleting %d \n",client_ind);

    clients[client_ind] = NULL;
    client_nicknames[client_ind] = NULL;
    current_games[client_ind] = -1;
    
    if (waiting_for_game_ind == client_ind)
        waiting_for_game_ind = -1; 

}


void * ping_thread(void * arg)
{


    while(TRUE)
    {
        sleep(10);
        pthread_mutex_lock(&mutex);
        for(int i = 0; i < last_client; i++)
        {
            if ( clients[i] != NULL)
            {
                if ( responding[i] == 1)
                {
                    net_message response;
                    response.message_type = PING;

                    if(sendto(remote_socket_fd, &response, sizeof(response) ,0, clients[i], sizeof(*clients[i])) == -1)
                    {
                        error("send");
                    }
                }
                else
                {
                    delete_client(i);
                }
                
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}


int register_name(char * nickname)
{
    printf("Nickname %s \n", nickname);
    for(int i = 0; i < last_name; i++)
    {
        if (client_nicknames[i] != NULL && strcmp(client_nicknames[i], nickname) == 0)
        {
            printf("Nickname %s \n", client_nicknames[i]);
            return -1;
        }
    }
    client_nicknames[last_name] = (char *) calloc(strlen(nickname), sizeof(char));
    strcpy(client_nicknames[last_name++], nickname);
    return 0;
}


void process_message(net_message message, struct sockaddr * address, int fd)
{

    if(message.message_type == WAITING_FOR_NICKNAME)
    {
        int ind;
        if ((ind = get_index_by_address(address)) == -1)
        {
            pthread_mutex_lock(&mutex);
            clients[last_client] = (struct sockaddr *) calloc(1, sizeof(struct sockaddr));
            memcpy(clients[last_client], address, sizeof(*address));
            responding[last_client] = 1;
            ind = last_client;
            last_client++;
            pthread_mutex_unlock(&mutex);
            
        }
        char nickname[MAX_NICKNAME_LENGTH];
        strcpy(nickname, message.message_text);

        if(register_name(nickname) != 0)
        {
            printf("Serio \n");
            net_message response;
            response.message_type = NICKNAME_OCCUPIED;

            if(sendto(fd, &response, sizeof(response) ,0, address, sizeof(*address)) == -1)
            {
                error("send name");
            }

        }
        else
        {
            net_message response;
            response.message_type = NICKNAME_ACCEPTED;

            if(sendto(fd, &response, sizeof(response) ,0, address, sizeof(*address)) == -1)
            {
                error("send name");
            }

            if(waiting_for_game_ind == -1)
            {
                waiting_for_game_ind = ind;

                net_message response;
                response.message_type = WAITING_FOR_OTHER_PLAYER;

                if(sendto(fd, &response, sizeof(response) ,0, address, sizeof(*address)) == -1)
                {
                    error("send accept");
                }

            }
            else
            {
                net_message start_game;
                start_game.message_type = PLAYING;
                
                for (int i = 0; i < 9; i++)
                    start_game.game_arr[i] = EMPTY;


                start_game.marker = X_marker;
                current_games[ind] = waiting_for_game_ind;
                current_games[waiting_for_game_ind] = ind;
                    
                int choice = rand() % 2;
                if (choice == 0)
                {
                    strcpy(start_game.message_text,client_nicknames[current_games[ind]]);   
                    if(sendto(fd, &start_game, sizeof(start_game) ,0, address, sizeof(*address)) == -1)
                    {
                        error("send");
                    }
                
                }               
                else 
                {
                    strcpy(start_game.message_text,client_nicknames[ind]);   
                    if(sendto(fd, &start_game, sizeof(start_game) ,0, clients[current_games[ind]], sizeof(*clients[current_games[ind]])) == -1)
                    {
                        error("send");
                    }
                }

                waiting_for_game_ind = -1;

            }
                

            printf("Hello %s\n", nickname);
            memset(nickname, 0, MAX_NICKNAME_LENGTH * sizeof(char));
            
        }
          
    }
    else if (message.message_type == QUIT)
    {  
        int ind = get_index_by_address(address);
        
        if (current_games[ind] != -1 &&  clients[current_games[ind]] != NULL)
        {
            if (message.marker == X_marker)
                message.marker = O_marker;
            else if(message.marker != LOST)
                message.marker = X_marker;


            memset(message.message_text,0, strlen(message.message_text) * sizeof(char));
            strcpy(message.message_text, client_nicknames[current_games[ind]]);

            if(sendto(fd, &message, sizeof(message) ,0, clients[current_games[ind]], sizeof(*clients[current_games[ind]])) == -1)
            {
                error("send");
            }
        }

        printf("Closing connection %d \n", get_index_by_address(address));     
        pthread_mutex_lock(&mutex);          
        delete_client(get_index_by_address(address));
        pthread_mutex_unlock(&mutex);

        


        /*
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
        */


    }
    else if (message.message_type == PING)
    {
        int ind = get_index_by_address(address);
        responding[ind] = 1;

        if (current_games[ind] != -1)
        {
            net_message kill_messsage;
            memset(kill_messsage.message_text,0, strlen(message.message_text) * sizeof(char));
            strcpy(kill_messsage.message_text, client_nicknames[current_games[ind]]);



            int other_ind = current_games[ind];
            kill_messsage.message_type = QUIT;
            kill_messsage.marker = LOST;
            if(sendto(fd, &kill_messsage, sizeof(kill_messsage), 0, clients[other_ind], sizeof(*clients[other_ind])) == -1)
            {
                error("Send kill");
            } 
        }


    }
    else if (message.message_type == PLAYING)
    {
        int ind = get_index_by_address(address);
        printf("%d and %d are playing \n", ind , current_games[ind]);

        if (message.marker == X_marker)
            message.marker = O_marker;
        else if(message.marker != LOST)
            message.marker = X_marker;


        memset(message.message_text,0, strlen(message.message_text) * sizeof(char));
        strcpy(message.message_text, client_nicknames[current_games[ind]]);



        if(sendto(fd, &message, sizeof(message) ,0, clients[current_games[ind]], sizeof(*clients[current_games[ind]])) == -1)
        {
            error("send");
        }


    }

            
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
        
        int local_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
        if ( bind(local_fd, (struct sockaddr*)&addr, sizeof(addr)) != 0)
            error("Unix bind \n");


        for ( int i = 0; i < MAX_CLIENTS; i++)
        {
            client_nicknames[i] = NULL;
            current_games[i] = -1;
        }

        struct addrinfo hints;
        memset(&hints, 0, sizeof (struct addrinfo));

        hints.ai_family = AF_INET;   
        hints.ai_socktype = SOCK_DGRAM;
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




        epoll_desc = epoll_create1(0);

        if(epoll_desc == -1)
        {
            error("epoll_desc");
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

            for (int i = 0; i < event_count; i++)
            {
                printf("Processing \n");
                struct epoll_event event_poll = events[i];
                struct epoll_mess * message = event_poll.data.ptr;


                struct sockaddr their_addr; 
                socklen_t their_size = sizeof(struct sockaddr);


                net_message mess;
                recvfrom(message->fd, &mess, sizeof(mess), MSG_WAITALL,&their_addr, &their_size); 
                process_message(mess, &their_addr, message->fd);

            }

        }


        
    }

    return 0;

}