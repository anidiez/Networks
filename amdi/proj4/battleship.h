#ifndef BATTLESHIP_H
#define BATTLESHIP_H

#include<stdio.h>
#include<signal.h>
#include<sys/time.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<fcntl.h>

#define DEF_PORT 3333
//MAXBUFLEN 4 bytes of header  + 512 bytes data + null terminate
#define MAXBUFLEN 517

typedef enum {
HIT = 1,
SHIP = 2,
GAME_DATA = 3,
ERROR = 4,
} packet_type;

#endif
