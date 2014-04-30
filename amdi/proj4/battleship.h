#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<signal.h>
#include<sys/time.h>
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

typedef enum {
HIT = 1,
SHIP = 2,
GAME_DATA = 3,
ERROR = 4,
} packet_type;

