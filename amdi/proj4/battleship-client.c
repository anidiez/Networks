//Creators Ana Diez and Steven Basart

//Libraries to include
#include "battleship.h"

//--Predefined

//Declare the functions
//void

int convertPortToInt(int port) {
  char c;
  int index, size = strlen(DEF_PORT);

  index = 0;
  while (size > 0) {
    c = DEF_PORT[size];
    port = (c - '0')*(index *10);
    index++;
    size--;
  }
  printf("port = %d\n",port);

}


//The starting point
int main() {
  int port, sockfd;
  static struct sockaddr_in server_addr;

  printf("Getting ready to play Battleship\n");

  port = convertPortToInt(port);

  //Try to establish a connection
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    perror("Socket failed to connect");
    exit(1);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);

}
