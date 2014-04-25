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

#define DEF_PORT 3335
//MAXBUFLEN 4 bytes of header  + 512 bytes data + null terminate
#define MAXBUFLEN 517
//when putting data into stuff
#define MAXDATA 512

typedef enum {
RRQ = 1,
WRQ = 2,
DATA = 3,
ACK = 4,
ERROR = 5
} packet_type;

#endif
