/*
**Battleship server
**
** Written by Ana Diez and Steven Basart
*/

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
//May not need all of these includes
#include<unistd.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<time.h>

#define DEF_PORT "3333"
#define MAX_PLAYERS 500

void setupServer(char* );
void setupGame(int player1, int player2);
void play();

int main(int argc, char* argv[]){
  int sockfd, index;
  char port[6];

  //zero it out
  memset(port, 0, 6);
  //
  strcpy(port,DEF_PORT);

  sockfd = setupServer(sockfd, port);

  for(index = 0; index < MAXCONNECTIONS; index++){
    clients[index] = -1;
  }

  int playerNum = 0;

  while(1){
    int player1 = -1, player2 = -1;

    addr_len = sizeof(client);
    clients[playerNum] = accept(sockfd, (struct sockaddr*) &client, &addr_len);

    if(clients[playerNum] != -1) {
      if(player1 < 0 && player2 < 0) {
        time(&timer1);
        player1 = clients[playerNum];
      } else {
        time(&timer2);
        player2 = clients[playerNum];
      }
    }
    if(clients[playerNum] < 0) {
      perror("error accepting client");
    }else{
      if (player1 > 0 && player2 > 0) {
        if(fork() == 0) {
          setupGame(player1,player2);
          exit(EXIT_SUCCESS);
        } else {
          player1 = -1;
          player2 = -1;
        }
      }
    }
    while(clients[index] !=-1){
      index = (clientNo+1)%MAXCONNECTIONS;
    }
  }
}

//Same setup as http
void setupServer(int sockfd, char* port) {
//server of fixed pages
    struct addrinfo hints;
    struct addrinfo *result, *rp;

//addrinfo for host
    memset(&hints,0,(sizeof(hints)));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if(getaddrinfo(NULL, port, &hints, &result) != 0){
        perror("getaddrinfo error");
    }

    for( rp = result; rp!=NULL; rp=rp->ai_next){
        sockfd = socket(rp->ai_family, rp->ai_socktype,0);
        if(sockfd == -1){
            //if failure go to next iteration
            continue;
        }
        //set socket fd to nonblocking
        if (fcntl(s, F_SETFL, O_NONBLOCK) < 0) {
          printf("Error setting nonblocking");
          exit(EXIT_FAILURE);
        }
        if(bind(sockfd, rp->ai_addr, rp->ai_addrlen) == 0){
            break;
        }
    }

    if(rp == NULL){
       perror("socket or bind error");
       exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

//listen for connections
    if(listen(sockfd,MAX_PLAYERS) != 0){
        perror("error listening for connectons");
        exit(EXIT_FAILURE);
    }
}


