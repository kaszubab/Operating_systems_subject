#define _GNU_SOURCE

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include "signal.h"
#include "shared.h"


int remote_socket_fd;
struct sockaddr * address;
socklen_t addrlen;


void help_function()
{
    printf("\nUsage :  ./client <nickname> <mode> <socket> \n");
    printf("nickname - unique name of the player (max 30 characters) \n");
    printf("mode - method of playing - either local or remote \n");
    printf("socket - ip address or path to the socket \n");

}


void error (char *msg)
{
    perror (msg);
    exit (1);
}

void sigint_handler(int sig)
{
    net_message kill_messsage;
    kill_messsage.message_type = QUIT;
    kill_messsage.marker = LOST;
    if(send(remote_socket_fd, &kill_messsage, sizeof(kill_messsage), 0) == -1)
    {
        error("Send kill");
    } 
    printf("Game is finished. Bye!\n");
    fflush(stdout);

    exit(EXIT_SUCCESS);
}

int connect_unix(char* path) {
  struct sockaddr_un addr, bind_addr;
  memset(&addr, 0, sizeof(addr));
  bind_addr.sun_family = AF_UNIX;
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, path, sizeof addr.sun_path);

  int sock = socket(AF_UNIX, SOCK_DGRAM, 0);	
  bind(sock, (void*) &bind_addr, sizeof addr);
  connect(sock, (struct sockaddr*) &addr, sizeof addr);

  return sock;
}

int connect_web(char* ipv4, int port) {
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  if (inet_pton(AF_INET, ipv4, &addr.sin_addr) <= 0) {
    error("Invalid address");
  }

  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  connect(sock, (struct sockaddr*) &addr, sizeof addr);
   
  return sock;
}






void visualize_board(int board[])
{
    char translation[9];

    for ( int i = 0; i < 9 ; i++)
    {
        if ( board[i] == X_marker)
            translation[i] = 'X';
        else if(board[i] == O_marker)
            translation[i] = 'O';
        else
            translation[i] = ' ';
    }

    printf("---------------------------\n");
    printf("-  %c    -   %c   -   %c  -\n",translation[0], translation[1], translation[2]);
    printf("---------------------------\n");
    printf("-  %c    -   %c   -   %c  -\n",translation[3], translation[4], translation[5]);
    printf("---------------------------\n");
    printf("-  %c    -   %c   -   %c  -\n",translation[6], translation[7], translation[8]);
    printf("---------------------------\n");

}

int check_table(int marker, int board[])
{
    int symbols_x;
    int symbols_y;
    for (int i = 0; i < 3; i++)
    {
        symbols_x = 1;
        symbols_y = 1;

        for (int j = 0; j < 3; j++)
        {
            symbols_x *= board[i*3 +j];
            symbols_y *= board[i + j * 3];
        }

        if(symbols_x == 1 || symbols_y == 1)
        {
            return O_marker;
        }
        if(symbols_x == 8 || symbols_y == 8)
            return X_marker;
    }
    symbols_x = 1;
    symbols_y = 1;

    for(int i = 0; i <3; i++)
    {
        symbols_x *= board[i*3 + i];
        symbols_y *= board[2-i + i*3];
    }

    if(symbols_x == 1 || symbols_y == 1)
        {
            return O_marker;
        }
        if(symbols_x == 8 || symbols_y == 8)
            return X_marker;

    return EMPTY;
}

int check_draw( int board[])
{
    for (int i = 0; i < 9; i++)
    {
        if(board[i] == EMPTY)
            return 0;
    }
    return 1;
}



int main (int argc, char **argv)
{
    if (argc != 4 && argc != 2) {
        fprintf (stderr, "Usage: client hostname\n");
        exit (EXIT_FAILURE);
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
    
    if (argc == 4)
    {

        if(signal(SIGINT, sigint_handler) == SIG_ERR)
        {
            perror("SIGINT handling couldn't be implemented");
            exit(EXIT_FAILURE);
        }

        if (strcmp(argv[2], "socket") == 0)
        {
            char * path = argv[3];
            remote_socket_fd = connect_unix(path);


        }
        else if(strcmp(argv[2], "ipv4") == 0)
        {
            char * server_address = strtok(argv[3], ":");
            char * port_number = strtok(NULL, ":");

            remote_socket_fd = connect_web(server_address, atoi(port_number));

        }

         printf("Socket_id %d \n", remote_socket_fd);


        int name_set = 1;

        net_message initial;
        initial.message_type=WAITING_FOR_NICKNAME;
        strcpy(initial.message_text, argv[1]);
        if (send (remote_socket_fd, &initial, sizeof(initial), 0) == -1)
            error ("send");

        net_message answer;
        if (recv (remote_socket_fd, &answer, sizeof (answer), 0) == -1)
            error ("recv");

        if(answer.message_type == NICKNAME_ACCEPTED)
        {
            name_set = 0;
            printf("Nickname accepted \n");
        }

        

        while (1) 
        {

            if (name_set == 1)
            {
                char message[MAX_NICKNAME_LENGTH];
                printf("ENTER A NICKNAME \n");
                scanf("%s", message);

                net_message initial;
                initial.message_type=WAITING_FOR_NICKNAME;
                strcpy(initial.message_text, message);
                if (send (remote_socket_fd, &initial, sizeof(initial), 0) == -1)
                    error ("send");

                
                // receive response from server
                net_message answer;
                if (recv (remote_socket_fd, &answer, sizeof (answer), 0) == -1)
                    error ("recv");

                if (answer.message_type == NICKNAME_OCCUPIED)
                {
                    continue;
                }
                else if(answer.message_type == NICKNAME_ACCEPTED)
                {
                    name_set = 0;
                    printf("Nickname accepted \n");
                }
                
                
            }
            if (name_set == 0)
            {
                net_message answer;
                if (recv (remote_socket_fd, &answer, sizeof (answer), 0) == -1)
                    error ("recv");

                if(answer.message_type == WAITING_FOR_OTHER_PLAYER)
                {
                    printf("Waiting for other player \n");
                }
                else if(answer.message_type == PLAYING)
                {
                    
                    printf("Currently playing with %s \n", answer.message_text);
                    if ( answer.marker == X_marker)
                        printf("Your marker is X \n");
                    else
                        printf("Your marker is O \n");

                    visualize_board(answer.game_arr);

                    int res = check_table(answer.marker, answer.game_arr);
                    if ( res != EMPTY)
                    {
                        if( res == answer.marker)
                            printf("You won. Congratulations \n");
                        else 
                            printf("You lost. Better luck next time \n");
                        
                    
                        answer.message_type = QUIT;
                        if (send (remote_socket_fd, &answer, sizeof(answer), 0) == -1)
                        error ("send");

                        printf("Game is finished. Bye!\n");
                        fflush(stdout);

                        exit(EXIT_SUCCESS);
                    }
                    else if(check_draw(answer.game_arr))
                    {
                        printf("Its a draw \n");
                        answer.message_type = QUIT;
                        if (send (remote_socket_fd, &answer, sizeof(answer), 0) == -1)
                        error ("send");

                        printf("Game is finished. Bye!\n");
                        fflush(stdout);

                        exit(EXIT_SUCCESS);
                    }
                    
                    

                    
                    printf("Select next not occupied position");
                    int pos;
                    scanf("%d", &pos);

                    while(answer.game_arr[pos] != EMPTY)
                    {
                        printf("Select next not occupied position \n");
                        scanf("%d", &pos);
                    }

                    answer.game_arr[pos] = answer.marker;
                    

                    if (send (remote_socket_fd, &answer, sizeof(answer), 0) == -1)
                    error ("send");

                }
                else if(answer.message_type == QUIT)
                {
                    printf("Currently playing with %s \n", answer.message_text);

                    if(answer.marker  == LOST)
                    {
                        printf("Other player left \n");
                        if (send (remote_socket_fd, &answer, sizeof(answer), 0) == -1)
                        error ("send");

                        printf("Game is finished. Bye!\n");
                        fflush(stdout);

                        exit(EXIT_SUCCESS);
                    }


                    if ( answer.marker == X_marker)
                        printf("Your marker is X \n");
                    else
                        printf("Your marker is O \n");
                    
                    visualize_board(answer.game_arr);  
                    int res = check_table(answer.marker, answer.game_arr);

                    if( res == answer.marker)
                        printf("You won. Congratulations \n");
                    else if(res != EMPTY)
                        printf("You lost. Better luck next time \n");
                    else
                    {
                        printf("Its a draw \n");
                    }
                    
                    
                    answer.message_type  =QUIT;
                    if (send (remote_socket_fd, &answer, sizeof(answer), 0) == -1)
                    error ("send");

                    printf("Game is finished. Bye!\n");
                    fflush(stdout);

                    exit(EXIT_SUCCESS);

                }

            }



        }
        
    
    }

    exit (EXIT_SUCCESS);
}

