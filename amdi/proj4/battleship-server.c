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
#define MAX_PLAYERS 100

int setupServer(int , char* );
void setupGame(int player1, int player2);
void play();

int main(int argc, char* argv[]){
  struct sockaddr_in client_addr;
  socklen_t addr_len;
  addr_len = sizeof(client_addr);
 
  int sockfd, index, playerNum;
  int player1 = -1, player2 = -1;
  char port[6];
  int players[MAX_PLAYERS];

  //for calculating how long to wait for the next user before trying again
  time_t timer1, timer2;

  //zero it out
  memset(port, 0, 6);
  //currently doesn't allow for changes without hard coding change
  strcpy(port, DEF_PORT);

  //not sure if we need to get back the changed sockfd or if its stored
  sockfd = setupServer(sockfd, port);

  for(index = 0; index < MAX_PLAYERS; index++){
    players[index] = -1;
  }


  while(1) {
    players[playerNum] = accept(sockfd, (struct sockaddr*) &client_addr, &addr_len);

    if(players[playerNum] != -1) 
    {
      if(player1 < 0 && player2 < 0) 
      {
        time(&timer1);
        player1 = players[playerNum];
      } 
      else 
      {
        time(&timer2);
        player2 = players[playerNum];
      }
    }
    if(players[playerNum] < 0) 
    {
      perror("error accepting client");
    } 
    else
    {
      if (player1 > 0 && player2 > 0) 
      {
        if(fork() == 0) 
        {
          setupGame(player1, player2);
          exit(EXIT_SUCCESS);
        } else {
          player1 = -1;
          player2 = -1;
          //Also reset players array players[] to -1;
        }
      }
    }
    while(players[playerNum] !=-1)
    {
      playerNum = (playerNum+1) % MAX_PLAYERS;
    }
  }
}

//Same setup as http
int setupServer(int sockfd, char* port) {
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
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0) {
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
   
  //Not sure if we need to return sockfd since it isn't global
  return (sockfd);
}

void setupGame(int player1, int player2) {
   //test stuff
   write(player1,"Hello num 1", 12);
   write(player2,"Hello num 2", 13);

   shutdown (player1, SHUT_RDWR);
   shutdown (player2, SHUT_RDWR);
   close(player1);
   close(player2);

}
