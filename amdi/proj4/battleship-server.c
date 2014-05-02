/*
**Battleship server
**
** Written by Ana Diez and Steven Basart
*/


//Searches current directory for battleship
#include "battleship.h"


//Definitions
#define MAX_PLAYERS 100

//global variables aren't the best idea but since we only use one socket...
//but Sutcliffe = my hero :/
//int sockfd;

//Globals
char player1Board[GAMEBOARD];
char player2Board[GAMEBOARD];


int setupServer(int sockfd, char* port);
void setupGame(int player1, int player2);
int getOpcode(char* packet);
int getBoatSize(shipps ship);
void makePacket(char* buf, int opcode, int position, char* data2);
void play(int, int);

int main(int argc, char *argv[]) {
  struct sockaddr_in client_addr;
  socklen_t addr_len;
  addr_len = sizeof(client_addr);
 
  int sockfd = -1, index;
  int playerNum = 0;
  int player1 = -1, player2 = -1;
  char port[6];
  int players[MAX_PLAYERS];

  //for calculating how long to wait for the next user before trying again
  time_t timer1, timer2;

  //zero it out
  memset(port, 0, 6);
  //currently doesn't allow for changes without hard coding change
  strcpy(port, DEF_PORT);

  //Probz need to get sockfd back
  sockfd = setupServer(sockfd, port);

  for(index = 0; index < MAX_PLAYERS; index++){
    players[index] = -1;
  }

  //Just so I can see what's happening.
  printf("waiting to accept a player...\n");
  //If you don't flush stdout it will wait until the server stuff has been
  //processed for some reason
  fflush(stdout);

  while(1) {
    players[playerNum] = accept(sockfd, (struct sockaddr*) &client_addr, &addr_len);
    if(players[playerNum] != -1) 
    {
      printf("someone tried to connect\n");
      fflush(stdout);
      if(player1 < 0 && player2 < 0) 
      {
        time(&timer1);
        player1 = players[playerNum];

       //set player1 socket fd to nonblocking
       if (fcntl(player1, F_SETFL, O_NONBLOCK) < 0) 
       {
         printf("Error setting nonblocking");
         exit(EXIT_FAILURE);
       }

        printf("player1 is here.\n");
        fflush(stdout);
      } 
      else 
      {
        time(&timer2);
        player2 = players[playerNum];
        
        //set player2 socket fd to nonblocking
        if (fcntl(player2, F_SETFL, O_NONBLOCK) < 0) 
        {
          printf("Error setting nonblocking");
          exit(EXIT_FAILURE);
        }

       printf("player2 is here. Let the games begin!\n");
       fflush(stdout);
      }
    }

    //Just wait a second
    sleep(1);

    if (player1 > 0 && player2 > 0) 
    {
      if(fork() == 0) 
      {
        printf("Setting up the game\n");
        fflush(stdout);
        setupGame(player1, player2);
        play(player1,player2);
        shutdown(player1, SHUT_RDWR);
        shutdown(player2, SHUT_RDWR);
        close(player1);
        close(player2);
        exit(EXIT_SUCCESS);
      } else {
        player1 = -1;
        player2 = -1;
        players[playerNum] = -1;
        players[playerNum-1] = -1;
      }
    }
    while(players[playerNum] !=-1)
    {
      playerNum = (playerNum+1) % MAX_PLAYERS;
    }
  }
  sutdown(sockfd, SHUT_RDWR);
  close(sockfd);
  exit(1);
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
   
  //We probably need to return sockfd since it isn't global
  return (sockfd);
}

//int PlaceShip (int ship, int loc, int ori, int playerNum)


int ParseShipData(char *input, int playerNum) 
{
  int i = 0, size = -1, start = -1, orientation = -1, interval = -1;

  char temp[3];

  strncpy(temp,input + 1,1);
  shipps ship = atoi(temp);

  //starting spot
  strncpy(temp, input+2,2);
  start = atoi(temp);

  //if right start from starting and add 1 until length reached
  orientation = strncmp(input+5,"1",1);

  if(orientation  == 0){
    interval = 10;
  } 
  orientation = strncmp(input+5,"0",1);

  if (orientation == 0) {
  //else if down start from starting and add 10 until length reached
    interval = 10;
  }
  //mark in arrays
  size = getBoatSize(ship);
  
  //Error Checking
  if (size < 0 || interval < 0 || start < 0 || start > 100) 
  {
    return(-1);
  }

  //Currently treats all boats the same maybe copy over client code for ship
  //placement later in case we want to distinguish 
  //PlacingShip****************************

  if(playerNum == 1) 
  {
    for(i = 0; i < size; i++)
    {
      player1Board[start + (interval*i)] = 'b';
    }
  }else if (playerNum == 2) 
  {
    for(i = 0; i < size; i++)
    {
      player2Board[start + (interval*i)] = 'b';
    }
  } 
  else 
  {
    printf("What kind of playerNum is that?");
    fflush(stdout);
    return (-1);
  }

  return(1);
}

void setupGame(int player1, int player2) {
  char p1buf[MAX_BUFF_LEN];
  char p2buf[MAX_BUFF_LEN];
  char buf[MAX_BUFF_LEN];
  int numRead = -1, opCode, index, temp, placed = 0;

  //Set both char arrays to '0'
  for(index = 0; index < GAMEBOARD; index++) {
    player1Board[index] = '0';
    player2Board[index] = '0';
 }

  //Not sure if write needs the char length + NULL terminator
  write(player1,"Hello player 1\n", 15);
  write(player2,"Hello player 2\n", 16);

  while (placed < 10) 
  {
    #ifdef debug
    printf("loop time getting boats");
    fflush(stdout);
    #endif

    numRead = read(player1, p1buf, MAX_BUFF_LEN);
    if(numRead > 0){
      printf("%s\n",p1buf);
      fflush(stdout);
      opCode = getOpcode(p1buf);
      if(opCode == SHIP){
        //save ship
        temp = ParseShipData(p1buf, 1);
        if(temp	 != -1){
          placed++;
          makePacket(buf, ACK, opCode, "");
        }else{
          makePacket(buf, ERROR, 0,"error: cannot parse ship data");
        }
        write(player1,buf,strlen(buf));
        //return ack
      }else if(opCode == ERROR){
        //print error message
        printf("%s\n",p1buf + 1);
        fflush(stdout);
        //exit -- let other client know of error
        makePacket(buf, ERROR,0, "error: opposing player error");
        write(player2, buf, strlen(buf));
        exit(EXIT_SUCCESS);
        //maybe close socket
      }else{
        //send error
      }
      numRead = -1;
    }

    //player2 stuff
    numRead = read(player2, p2buf, MAX_BUFF_LEN);
    if(numRead > 0){
      printf("%s\n",p2buf);
      fflush(stdout);
      opCode = getOpcode(p2buf);
      if(opCode == SHIP){
        //save ship
      temp = ParseShipData(p2buf, 2);
        if(temp != -1){
          placed++;
          makePacket(buf, ACK, opCode, "");
        }else{
          makePacket(buf, ERROR, 0,"error: cannot parse ship data");
        }
        write(player2,buf,strlen(buf));
        //return ack
      }else if(opCode == ERROR){
        //print error message
        printf("%s\n",p2buf + 1);
        fflush(stdout);
        //exit -- let other client know of error
        makePacket(buf, ERROR,0, "error: opposing player error");
        write(player1, buf, strlen(buf));
        exit(EXIT_SUCCESS);
        //maybe close socket
      }else{
        //send error************************
      }
      numRead = -1;
    } 

    //small pause
    sleep(1);
  }   
  printf("made it out of setup loopi\n");
        fflush(stdout);
  return;
}

int getOpcode(char* packet){
  int opCode;
  char num[2];

  memset(num,0,2);
  strncpy(num,packet,1);
  opCode = atoi(num);
  return opCode;
}

//Ana is doing this for now MUAHAHAHAHA
int getBoatSize(shipps ship){
  int size;
  switch(ship){
    case A:
      size = 5;
      break;
    case B:
      size = 4;
      break;
    case C:
    case S:
      size = 3;
      break;
    case P:
      size = 2;
      break;
    default:
      size = -1;
      break;
  }
  return size;
}

//Ana is doing this
void makePacket(char* buf, int opCode, int position, char* data){
  memset(buf, 0, MAX_BUFF_LEN);
  switch (opCode){
    case GAME_DATA:
      //we're using data to pass in the hit or miss... 
      if(data[0] != '0' && data[0] != '1'){
        sprintf(buf, "%derror: invalid hit or miss\n",ERROR);
        fflush(stdout);
        return;
      }
      sprintf(buf, "%d%d;%c\n", opCode,position,data[0]);
        fflush(stdout);
      break;
    case ACK:
      //here we're just using position as the opcode we're confirming we got
      //don't overthink it it's just convenience of types
      sprintf(buf,"%d%d\n",opCode,position);
        fflush(stdout);
      break;
    case WIN:
    case TURN:
      //using position to represent turn value - 0 or 1
      if(position != 1 && position !=0){
        return;
      }
      sprintf(buf, "%d%d\n",opCode,position);
        fflush(stdout);
      break;
    case ERROR:
      sprintf(buf,"%d%s\n",opCode, data);
        fflush(stdout);
      break;
  }
  //we don't have to return anything because buf is passed in as pointer
  return;
}

//play is missing *********
void play(int player1, int player2)
{

  int p1deaths = 0, p2deaths = 0, current = -1, waiting = -1, holder = -1;
  int readBytes = 0, position, opCcheck;
  char buf[MAX_BUFF_LEN];

printf("made it into play\n");
        fflush(stdout);
  //send p1 turn packet, it's their turn
  makePacket(buf,TURN,1, "");
printf("sent this to p1%s\n",buf);
        fflush(stdout);
  write(player1,buf,strlen(buf));
  //send p2 turn packet, it's not their turn
  makePacket(buf,TURN,0, "");
printf("sent this to p2,%s\n",buf);
        fflush(stdout);
  write(player2,buf,strlen(buf));

  //set current player to p1
  current = player1;
  //set waiting player to p2
  waiting = player2;
  //start loop - while p1deaths and p2deaths are less than DEATH
  while(p1deaths < DEATH && p2deaths < DEATH){
    //wait for currentplayer's hit
    memset(buf,0, MAX_BUFF_LEN);
    readBytes = read(current, buf, MAX_BUFF_LEN);
    if(readBytes > 0){
printf("we got a reading from player in play\n");
fflush(stdout);
    //check for correct opcode
      opCcheck = strncmp(buf,"1",1);
      if(opCcheck == 0){
        //get position
        position = atoi(buf + 1);
        //check for the opposing player's boat array
        char check;
        if(current == player1){
          check = player2Board[position];
        }else{
          check = player1Board[position];
        }

        if(check == '0'){
        //if 0 send both players a gamedata miss
          makePacket(buf,GAME_DATA,0,"");
        }else if(check == 'b'){
        //if b send both players a gamedata hit and ++ opposing player's death
          if(current == player1){
            p2deaths++;
          }else{
            p1deaths++;
          }
          makePacket(buf, GAME_DATA,1,"");
        }else{
        //else send errors to players
          makePacket(buf, ERROR,0,"error: reading board");
          printf("something went wrong setting up the board");
          fflush(stdout);
          exit(EXIT_FAILURE);
        }
        write(player1,buf,strlen(buf));
        write(player2,buf,strlen(buf));
        //switch packets
        holder = current;
        current = waiting;
        waiting = current;
      }else{
          //received wrong type of packet... send error to current
        makePacket(buf, ERROR, 0, "error: unexpected opcode (packet type)");
        write(current, buf, strlen(buf));
          // no switching
      }
    }
    sleep(5);    
  }
  //send win lose messages
  if(p1deaths == DEATH){
    //player 1 died first
    makePacket(buf, WIN, 0, "");
    write(player1,buf,strlen(buf));
    makePacket(buf,WIN, 1, "");
    write(player2,buf,strlen(buf));
  }else if(p2deaths == DEATH){
    //player 2 died first
    makePacket(buf, WIN, 0, "");
    write(player2,buf,strlen(buf));
    makePacket(buf,WIN, 1, "");
    write(player1,buf,strlen(buf));
  }else{
    //wat
    makePacket(buf, ERROR, 0, "error: unexpected server error");
    write(player1,buf,strlen(buf));
    write(player2,buf,strlen(buf));
  }
  return;
}
