//Creators Ana Diez and Steven Basart

//Libraries to include
#include "battleship.h"

//--Predefined
#define TRUE 1
#define FALSE 0

//Declare the functions
//void
int powerN(int, int, int);
int convertPortToInt(int);
int setupGame();


int powerN(int number, int base,int exponent){
  if (exponent < 0) {
    return 0;
  }
  if (exponent == 0){
    return 1;
  } else if (exponent == 1) {
    return number;
  } else {
    return (powerN(base*number,base,exponent-1));
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
  return(port);
}


//The starting point
int main() {
  int port, sockfd;
  char buffer[MAX_BUFF_LEN];
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
  //Do we need this?
  //server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  server_addr.sin_port = htons(port);


    //---Try to establish connection
  if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    perror("Connection Failed");
    exit(1);
  }
  
  setupGame(sockfd);

}

void setupGame(int sockfd) {
  int index = 0, readStatus = 0;
 
  while (TRUE) {
    if (index = 0){ 
      write(sockfd, "Hello Server",13);
      index = 1;
    }
    readStatus = read(sockfd, buffer, MAX_BUFF_LEN);
    if (readStatus > 0) {
      //What does write(1 ...) do? 
      //I think it writes to stdout
      //write(1,buffer,readStatus);

      printf("And the server says: %s",buffer);
    }

  }
 

}

