//Creators Ana Diez and Steven Basart

//Libraries to include
#include "battleship.h"
//--Predefined

//Declare the functions
//void

int powerN(int powNumber, int number,int times){
  if (times == 0){
    return 1;
  } else if ( times == 1) {
    return powNumber;
  } else {
    return (powerN(powNumber*number,number,times-1));
  }
}

int convertPortToInt(int port) {
  char c;
  int index, size = strlen(DEF_PORT);

  size = size -1;
  index = 0;
  while (size >= 0) {
    c = DEF_PORT[size];
    port += (c - '0')*(powerN(10,10,index));
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
