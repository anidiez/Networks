#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<signal.h>
#include<time.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<fcntl.h>

#define DEF_PORT "3380"
//MAXBUFLEN 4 bytes of header  + 512 bytes data + null terminate
#define MAX_BUFF_LEN 517
#define GAMEBOARD 100

#define MAX_PLAYERS 100

typedef enum {
HIT = 1,
SHIP = 2,
GAME_DATA = 3,
ACK = 4,
TURN = 5,
ERROR = 6,
} packet_type;
/*
typedef enum {
AIRCARRIER,
BATTLESHIP,
CRUISER,
SUBMARINE,
PATROL,
} boats;
*/
typedef enum {
A = 0,
B = 1,
C = 2,
S = 3,
P = 4,
} shipps;
