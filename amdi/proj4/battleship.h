#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

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

#define DEF_PORT 3333
//MAXBUFLEN 4 bytes of header  + 512 bytes data + null terminate
//probably will need to change this
#define MAXBUFLEN 517

#define MAX_PLAYERS 100

typedef enum {
HIT = 1,
SHIP = 2,
GAME_DATA = 3,
ERROR = 4,
} packet_type;

typedef enum {
AIRCARRIER,
BATTLESHIP,
SUBMARINE,
DESTROYER,
PATROL
}

#endif
