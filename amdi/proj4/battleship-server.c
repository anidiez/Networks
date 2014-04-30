/*
**Battleship server
**
** Written by Ana Diez and Steven Basart
*/

//Searches current directory for battleship
#include "battleship.h"
//May not need to include
#include<sys/stat.h>

//global variables aren't the best idea but since we only use one socket...
int sockfd;

int setupServer(int , char* );
void setupGame(int player1, int player2);
void play();

int main(int argc, char* argv[]){
  struct sockaddr_in client_addr;
  socklen_t addr_len;
  addr_len = sizeof(client_addr);
 
//  int sockfd;
    int index, playerNum;
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

  //Just so I can see what's happening.

  while(1) {
    players[playerNum] = accept(sockfd, (struct sockaddr*) &client_addr, &addr_len);
//why do we have a player1 and player2 if we also have an array of players?
//if we're doing what i think we're doing we have to reset p1 and p2 to -1 
//at some point - what we have right now will crash after the first 2 players
//also the ifs might not always work...because if both p1 and p2 are >0 we still
//get a p2... we only want to get a p2 if p1>0 and p2<0
    if(players[playerNum] != -1) 
    {
      if(player1 < 0 && player2 < 0) 
      {
        time(&timer1);
        player1 = players[playerNum];
        printf("player1 is not zero\n");
      } 
      else 
      {
        time(&timer2);
        player2 = players[playerNum];
        printf("player2 is not zero\n");
      }
    }

    //Just wait a second
    sleep(1);

    if (player1 > 0 && player2 > 0) 
    {
      if(fork() == 0) 
      {
        printf("Setting up the game\n");
        setupGame(player1, player2);
        exit(EXIT_SUCCESS);
      } else {
        player1 = -1;
        player2 = -1;
        //Also reset players array players[] to -1;
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
  //i made it global... it just makes sense since we're always using the same socket
  //return (sockfd);
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
