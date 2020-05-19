#ifndef queues
#define queues



#define TRUE 1
#define FALSE 0

#define MAX_CLIENTS     15
#define MAX_NICKNAME_LENGTH      50

#define LOST 7
#define PING 6
#define NICKNAME_ACCEPTED 5
#define NICKNAME_OCCUPIED 4
#define QUIT 3
#define WAITING_FOR_OTHER_PLAYER 2
#define PLAYING 1
#define WAITING_FOR_NICKNAME 0 

#define X_marker 2
#define O_marker 1
#define EMPTY 0




typedef struct net_message
{
    int message_type;
    int marker;
    int game_arr[9];
    char message_text[MAX_NICKNAME_LENGTH]; 
} net_message;



#endif