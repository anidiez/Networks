//Creators Ana Diez and Steven Basart

//Libraries to include
#include "battleship.h"

//Declare the functions
int   powerN(int, int, int);
int   convertPortToInt(int);
int   setupGame(int);
int   ParseInputSetup(int,char* input);
int   ParseInputHit(char* input);
int   ParseHitPacket(char*,int);
int   ParseTurnPacket (char *packet);
char* getUserInput();
void  displayBoard();
int   CheckBounds(int location, int length, int oriented);
int   CheckCollision(int location, int length, int oriented);
void  play(int);

//Globals
char shipArray[GAMEBOARD];
int  hitsArray[GAMEBOARD];
int  ships[5];



//The starting point
int main() {
  int port = 0, sockfd, status;
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
  
  status = setupGame(sockfd);
  status = whoseTurn(sockfd);
  play(sockfd);
}

//Raises the base to that exponent only ints
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
//Converts the char port to an int
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


//Gets the input from the user
char* getUserInput() 
{
  int max = 20;
  char* input = (char*)malloc(max); // allocate buffer
  if (input == 0) {exit(0);}

  printf("Enter boat and orientation: \n>");

  while (true) // skip leading whitespace
  {
    int c = getchar();
    if (c == EOF || c == '\n') {break;} // end of file
    if (!isspace(c)) 
    {
      ungetc(c, stdin);
      break;
    }
  }

  int i = 0;
  while (true) 
  {
    int c = getchar();
    if (c == EOF || c == '\n') // at end, add terminating zero
    {
      input[i] = '\0';
      break;
    }
    input[i] = c;
    if (i==max-1) // buffer full
    { 
      max = max+max;
      input = (char*)realloc(input,max); // get a new and larger buffer
      if (input == 0) {exit(0);}
    }
    i++;
  }

  #ifdef debug
  printf("The input is %s\n", input);
  #endif
  return input;
}

//If type = 0 it'll display shipArray if type = 1 it'll display hits
void displayBoard(int type)
{
  int index = 0,index2 = 0;

  printf("   ");
  for(index = 0; index < 10; index++) {
    printf("  %c ",('A'+index));
  }
  printf("\n");

  for(index = 0; index < 10; index++) 
  {
    for(index2 = 0; index2 < 10; index2++) 
    {
      if(index2 == 0) 
      {
        printf(" %d ",index);
      }

      //type 0 equals my ship states
      if (type == 0) {
        printf(" [%c]",shipArray[index*10+index2]);
      } 

      //type 1 equals my hits
      else if (type == 1)
      {
        if (hitsArray[index*10+index2] == 0)
        {
          printf(" [O]");
        }
        else if(hitsArray[index*10+index2] == 1)
        {
          printf(" [X]");
        }
        else if(hitsArray[index*10+index2] == 2)
        {
          printf(" [!]");          
        }
        else
        {
          printf("something went wrong in setting up hits\n");
          return;
        }
      } 
      else
      {
        printf("Sorry invalid display type\n");
        return;
      }

    }
    printf("\n");
  }
}


//down == 0
int CheckBounds(int location, int length, int oriented) 
{
  int tens, ones;
  tens = location/10;
  ones = location % 10;

  #ifdef debug
  printf("The loc = %d len = %d ori = %d tens = %d ones =%d \n",\
location,length,oriented,tens,ones);
  fflush(stdout);
  #endif

  //if going to the down
  if (oriented == 0)
  {
    //11 because 10 can exist
    if((length + tens) > 11 ) 
    {
      printf("Sorry ship out of bounds. Try again\n");
      return (-1);
    }
  } 
  //if going right
  if(oriented == 1) 
  {
    //11 because 10 can exist
    if((length + ones) > 11 ) 
    {
      printf("Sorry ship out of bounds. Try again\n");
      return (-1); 
    }
  }
  return (1);
}

int CheckCollision(int location, int length, int oriented)
{
  int tens,ones,index = 0;
  tens = location/10;
  ones = location % 10;

  //if going down
  if(oriented == 0) 
  {
    for(index = 0; index < length; index++) 
    {
      //11 because 10 can exist
      if((shipArray[location + (10*index)]) != '0') 
      {
        printf("Sorry can't place ships on top of one another. Try again\n");
        return (-1);
      }
    }
  }
  //if going to the right
  if (oriented == 1)
  {
    for(index = 0; index < length; index++) 
    {
      //11 because 10 can exist
      if((shipArray[location + index]) != '0') 
      {
        printf("Sorry can't place ships on top of one another. Try again\n");
        return (-1);
      }
    }
  } 
  return (1);
}

//down == 0
void PlaceShip(char shippie, int location, int size, int oriented)
{
  int index = 0;

  //if going down
  if(oriented == 0) 
  {
    for(index = 0; index < size; index++) 
    {
      shipArray[location + (10*index)] = shippie;
    }
  }
  //if going to the right
  if (oriented == 1)
  {
    for(index = 0; index < size; index++) 
    {
      shipArray[location + index] = shippie;
    }
  } 

}

int CheckAlreadyUsed(int shippie) 
{
  int index;
  if (ships[shippie] == 0)
  {
    ships[shippie] = 1;
    return(1);
  } else {
    //Add a switch statement to print out which boat was already placed ******
    printf("Sorry ship already used\n");
    return(-1);
  }
} 


int ParseInputHit(char* input)
{
  int i = 0, c, index = 0, hit = -1;

  #ifdef debug  
  printf("Look at me parse hits\n");
  printf("The input should still be %s\n",input);
  fflush(stdout);
  #endif

  while (true) // skip leading whitespace
  {
    c = input[i];
    
    #ifdef debug
    printf("what is c %c\n",c);
    fflush(stdout);
    #endif

    if (c == EOF || c == '\n') {break;} // end of file
    if (!isspace(c)) 
    {
      if(i != 0)
      {
        i--;
      }
      break;
    }
    i++;
  }

  #ifdef debug
  printf("we're halfway ~parsed c is %c i is %d\n",c,i);
  fflush(stdout);
  #endif

  while (true) 
  {
    c = input[i];

    #ifdef debug
    printf("c is %c index is %d i is %d\n",c, index, i); 
    fflush(stdout);
    #endif

    if (isspace(c) || c == ',') 
    {
      i++;
      continue;
    }
    if (c == EOF || c == '\n' || c == '\0') // at end, add terminating zero
    {
      #ifdef debug
      printf("what is c before break %c %d i = %d\n",c, (int)c, i);
      printf("What are these as ints EOF %d \\n %d \\0%d\n",(int)EOF,\
(int)('\n'), (int)('\0'));
      #endif

      break;
    }

    //sets location horizontally
    if (index == 0)
    { 
      #ifdef debug
      printf("before c %c index = %d loc = %d\n",c,index,location);
      #endif

      c = toupper(c);
      hit = c - 'A';
      #ifdef debug
      printf("after upper c %c index = %d loc = %d\n",c,index,location);
      #endif
      i++;

    }
    //sets location vertically and adds them
    if (index == 1)
    { 
      hit += 10*((int)((char)c-'0'));

      #ifdef debug
      printf("final loc c %c index = %d loc = %d\n",c,index,location);
      #endif

      //Check if hit is in range
      if (hit < 0 || hit > 100) {
        printf("Sorry hit was not in range. Try again\n");
        return (-1);
      }
    }
    index++;
  }
  //before returning hit check if already done
  if (hitsArray[hit] != 0)
  {
    printf("All of there ships are anchored into place.\n");
    printf("So there's no need to shoot the same spot twice.\n");
    return(-1);
  }
  return hit;
}

//Parses input and writes to server returns 1 on success and -1 on failure
int ParseInputSetup(int sockfd, char* input)
{
  int i = 0, c, index = 0, location, oriented, returnValue = -1;
  char ship, orientation, check;
  char string[MAX_BUFF_LEN];

  #ifdef debug  
  printf("Look at me parse\n");
  printf("The input should still be %s\n",input);
  fflush(stdout);
  #endif

  while (true) // skip leading whitespace
  {
    c = input[i];
    
    #ifdef debug
    printf("what is c %c\n",c);
    fflush(stdout);
    #endif

    if (c == EOF || c == '\n') {break;} // end of file
    if (!isspace(c)) 
    {
      if(i != 0)
      {
        i--;
      }
      break;
    }
    i++;
  }

  #ifdef debug
  printf("we're halfway ~parsed c is %c i is %d\n",c,i);
  fflush(stdout);
  #endif

  while (true) 
  {
    c = input[i];

    #ifdef debug
    printf("c is %c index is %d i is %d\n",c, index, i); 
    fflush(stdout);
    #endif

    if (isspace(c) || c == ',') 
    {
      i++;
      continue;
    }
    if (c == EOF || c == '\n' || c == '\0') // at end, add terminating zero
    {
      #ifdef debug
      printf("what is c before break %c %d i = %d\n",c, (int)c, i);
      printf("What are these as ints EOF %d \\n %d \\0%d\n",(int)EOF,\
(int)('\n'), (int)('\0'));
      #endif

      break;
    }
    //sets ship
    if (index == 0) 
    { 
      c = toupper(c); 
      ship = (char)c;
      #ifdef debug
      printf("what is c %c index = %d ship = %c\n",c,index, ship);
      #endif

      //Clear the rest of the chars after letter and before ,
      while(c != ((int)',') && c != EOF && c != ((int)'\n') && c != ((int)'\0'))
      {
        #ifdef debug
        printf("what is c now %c %d\n",c, (int)c);
        #endif

        i++;
        c = input[i];
      }
    }
    //sets location horizontally
    if (index == 1)
    { 
      #ifdef debug
      printf("before c %c index = %d loc = %d\n",c,index,location);
      #endif

      c = toupper(c);
      location = c - 'A';
      #ifdef debug
      printf("after upper c %c index = %d loc = %d\n",c,index,location);
      #endif
      i++;

    }
    //sets location vertically and adds them
    if (index == 2)
    { 
      location += 10*((int)((char)c-'0'));

      #ifdef debug
      printf("final loc c %c index = %d loc = %d\n",c,index,location);
      #endif

    //Clear the rest of the chars after letter and before ,
      while(c != (int)',' && c != EOF && c != (int)'\n' && c != (int)'\0') 
      {
        i++;
        c = input[i];
      }

    }
    //sets orientation
    if (index == 3)
    { 
      orientation = (char)c;
      #ifdef debug
      printf("what is c %c index = %d ori = %c\n",c,index,orientation);
      #endif

      //Clear the rest of the chars after letter and before ,
      while(c != (int)',' && c != EOF && c != (int)'\n' && c != (int)'\0') 
      {
        i++;
        c = input[i];
      }
    }
    index++;
  }

  #ifdef debug
  printf("what is c %c and i = %d\n",c,i);
  printf("ship = %c orient = %c loc = %d\n",ship, orientation, location);
  fflush(stdout);
  #endif


  if(index < 3)
  {
    printf("index was less than three \n");
    return(-1);
  }
  if (orientation == 'd' || orientation == 'D')
  {oriented = 0;}
  else if (orientation == 'r' || orientation == 'R')
  {oriented = 1;}
  else 
  {
    printf("Invalid orientation. Only right or down.\n");
    return(-1);
  }

  #ifdef debug
  printf("Am I passed orientation? oriented = %d\n", oriented);
  fflush(stdout);
  #endif


//Need to check if I used the enums correctly here*******************

  switch (ship) 
  {
    case 'A':
    case 'a':
      if(CheckBounds(location, 5, oriented) < 0)
      {return (-1);}
      if(CheckCollision(location, 5, oriented) < 0)
      {return(-1);}
      if(CheckAlreadyUsed(A) < 0)
      {return (-1);}
      PlaceShip(ship, location, 5, oriented);
      sprintf(string,"20%02d;%d",location,oriented);
      write(sockfd,string,strlen(string));
      break;

    case 'B':
    case 'b':
      if(CheckBounds(location, 4, oriented) < 0)
      {return (-1);}
      if(CheckCollision(location, 4, oriented) < 0)
      {return(-1);}
      if(CheckAlreadyUsed(B) < 0)
      {return (-1);}
      PlaceShip(ship, location, 4, oriented);
      sprintf(string,"21%02d;%d",location,oriented);
      write(sockfd,string,strlen(string));
      break;

    case 'C':
    case 'c':
      if(CheckBounds(location, 3, oriented) < 0)
      {return (-1);}
      if(CheckCollision(location, 3, oriented) < 0)
      {return(-1);}
      if(CheckAlreadyUsed(C) < 0)
      {return (-1);}
      PlaceShip(ship, location, 3, oriented);
      sprintf(string,"22%02d;%d",location,oriented);
      write(sockfd,string,strlen(string)); 
      break;

    case 'S':
    case 's':
      if(CheckBounds(location, 3, oriented) < 0)
      {return (-1);}
      if(CheckCollision(location, 3, oriented) < 0)
      {return(-1);}
      if(CheckAlreadyUsed(S) < 0)
      {return (-1);}
      PlaceShip(ship, location, 3, oriented);
      sprintf(string,"23%02d;%d",location,oriented);
      write(sockfd,string,strlen(string)); 
      break;

    case 'P':
    case 'p':
      if(CheckBounds(location, 2, oriented) < 0)
      {return (-1);}
      if(CheckCollision(location, 2, oriented) < 0)
      {return(-1);}
      if(CheckAlreadyUsed(P) < 0)
      {return (-1);}
      PlaceShip(ship, location, 2, oriented);
      sprintf(string,"24%02d;%d",location,oriented);
      write(sockfd,string,strlen(string));
      break;

    default:
      printf("Sorry invalid ship try again\n");
  }

  #ifdef debug
  printf("I'm about to return 1");
  fflush(stdout);
  #endif


  return(1);
}


//return success or fail
int setupGame(int sockfd) 
{
  char buffer[MAX_BUFF_LEN]; 
  int index = 0, readStatus = 0, placed = 0, hit = -1;
  char *input;

  //Initialize the game board
  for(index = 0; index < GAMEBOARD; index++) 
  {
    shipArray[index] = '0';
  }
  //Initialize the ships in Use
  for(index = 0; index < 5; index++) 
  {
    ships[index] = 0;
  }
  //Initialize the hits array
  for(index = 0; index < GAMEBOARD; index++) 
  {
    hitsArray[index] = 0;
  }

  index = 0; 

  printf("Waiting for other player\n");

  readStatus = read(sockfd, buffer, MAX_BUFF_LEN);
  if (readStatus > 0) 
  {
    printf("And the server says: %s",buffer);
  }

  printf("Place your ships     shipsize\n");
  printf("(A)ircraft carrier    = 5\n");
  printf("(B)attleship          = 4\n");
  printf("(C)ruiser             = 3\n");
  printf("(S)ubmarine           = 3\n");
  printf("(P)atrol              = 2\n");

  while (true) 
  {
    //Can't send null terminator over internet?
    memset(buffer,0,MAX_BUFF_LEN);

    printf("\nBoats are A, B, C, S, P\n");
    printf("Select boat, location, and orientation(down or right)\n");
    printf("Example: \"B, C5, down\"\n");
    fflush(stdout);
    //displayBoard 0 displays ship placements
    displayBoard(0);

    input = getUserInput();
    #ifdef debug
    printf("Did I survive getting input?\n");
    printf("And the input is %s\n",input);
    fflush(stdout);
    #endif

    //Parses the input to see if valid and avoid collisions
    index = ParseInputSetup(sockfd, input);

    #ifdef debug
    printf("Did I parse?\n");
    fflush(stdout);
    #endif

    if(index > 0) 
    {
      //reset index
      index = 0;
      readStatus = read(sockfd, buffer, MAX_BUFF_LEN);
      if (readStatus > 0) 
      {
        printf("And the server says: %s",buffer);
      }
      
      placed++;
      //All boats placed
      if (placed == 5) 
      { break;}
    }

    free(input); // release memory allocated for user
   
  }
  //#ifdef debug
  printf("Finished setting up\n");
 // #endif
}

int ParseTurnPacket (char *packet) 
{
  int i =0;

  if ((packet[0] - '0') == TURN)
  {
    return((int)packet[1]);
  } 
  else
  {
    printf("What kind of packet did I get?\n");
    return(-1);
  }
  
}

//Before going into game loop wait for ack from server to say its your turn
//if its your turn go into game loop (return 1) 
//else wait for info packet about other users hits
int whoseTurn (int sockfd) {
  int readStatus, turn = -1, hitted = -1;
  char buffer[MAX_BUFF_LEN]; 
 
  printf("Waiting for my turn");
  fflush(stdout);

  readStatus = read(sockfd, buffer, MAX_BUFF_LEN);
  if (readStatus > 0) 
  {
    printf("And the server says: %s",buffer);
    //Parse the input from the server
    turn = ParseTurnPacket(buffer);
  }

  #ifdef debug
  printf("Did I make it past read?\n");
  fflush(stdout);
  #endif

  if(turn == 1)
  {
    #ifdef debug
    printf("Am I returning?\n");
    #endif
    return;
  }

  //Player 2 waiting
    readStatus = read(sockfd, buffer, MAX_BUFF_LEN);
    if (readStatus > 0) 
    {
      printf("And the server says: %s",buffer);
      //Parse the input from the server
      hitted = ParseHitPacket(buffer,0);
    }
 
}

//type 0 is the shipArray Waiting to be hit
//turn = 0 Not my turn
int UpdateArray(int location, int hitMiss, int type)
{
  int c;

  if (type == 0)
  {
    //Simple approach shipArray[location] = 'X';
    if (shipArray[location] == '0')
    {
      shipArray[location] = 'X';
    }
    else 
    {
      c = shipArray[location];
      shipArray[location] = tolower((char)c);
    }
  }
  else if(type == 1)
  {
    if(hitMiss == 1)
    {
      hitsArray[location] = '!';
    } 
    else 
    {
      hitsArray[location] = 'X';
    }
  }
  else
  {
    printf("Some error");
    return(-1);
  }
  return (1);
}

//Where type will specify which array to update
//type 0 is the shipArray Waiting to be hit
//turn = 0 Not my turn
int ParseHitPacket(char *input, int type)
{
  int index = 0, i = 0, c, OpCode = -1, location = -1, hitMiss = -1;

  while(true)
  {
    c = input[i];

    if (isspace(c) || c == ';')
    {
      i++;
      continue;
    }
    if (c == EOF || c == '\n' || c == '\0') // at end, add terminating zero
    {
      #ifdef debug
      printf("what is c before break %c %d i = %d\n",c, (int)c, i);
      printf("What are these as ints EOF %d \\n %d \\0%d\n",(int)EOF,\
(int)('\n'), (int)('\0'));
      #endif

      break;
    }
    //sets opCode
    if (index == 0)
    {
      OpCode = (char)c - '0';
      #ifdef debug
      printf("what is c %c index = %d ship = %d\n",c,index, ship);
      #endif

    }
    //sets location horizontally
    if (index == 1)
    {
      #ifdef debug
      printf("before c %c index = %d loc = %d\n",c,index,location);
      #endif

      location = (char)c - '0';

      #ifdef debug
      printf("after upper c %c index = %d loc = %d\n",c,index,location);
      #endif
      i++;

    }
    //sets location vertically and adds them
    if (index == 2)
    {
      location += 10*((int)((char)c-'0'));

      #ifdef debug
      printf("final loc c %c index = %d loc = %d\n",c,index,location);
      #endif

    }
    //sets orientation
    if (index == 3)
    {
      hitMiss = (char)c -'0';

      #ifdef debug
      printf("what is c %c index = %d ori = %c\n",c,index,hitMiss);
      #endif
    }
    index++;

  }
  
  //Error Checking
  if (OpCode > 6 || OpCode < 0 || hitMiss < 0 || hitMiss > 1 || location < 0 ||\
location > 100) 
  {
    return(-1);
  }

  //Check return value of this ***********
  UpdateArray(location, hitMiss, type);

  return(1);
  
}

//The Game loop !!!! Woooo!! It's sooooo much FUN! LET'S NEVER STOP PLAYING 
void play(int sockfd) {
  int readStatus = -1, status = ERROR, hit = -1;
  char buffer[MAX_BUFF_LEN],string[20];
  char *input;

  while(true) 
  {
    printf("Select location to launch an Anti-Ship missile\n");
    printf("Example: \"C5\"\n");
    fflush(stdout);
    //display their past hits
    displayBoard(1);

    input = getUserInput();

    #ifdef debug
    printf("Did I survive getting input?\n");
    printf("And the input is %s\n",input);
    fflush(stdout);
    #endif

    //Parses the input to see if valid and avoid collisions
    hit = ParseInputHit(input);

    #ifdef debug
    printf("Did I parse?\n");
    fflush(stdout);
    #endif


    if(hit > -1) 
    {
      //For error checking we'll do it later****
      //while(status == error) {

        //Write hit to server ******************************************
        sprintf(string,"1%02d",hit);
        write(sockfd,string,strlen(string));

        //Receives whether or not the hit was a hit or miss
        readStatus = read(sockfd, buffer, MAX_BUFF_LEN);
        if (readStatus > 0) 
        {
          printf("And the server says: %s",buffer);
          //Parse hit for hit or miss
          status = ParseHitPacket(buffer,1);

          //May not be a hit packet in which case status == -1
          //Check for end game condition

          //More error checking

          //if(status == error){
          // continue; which will resend the packet
          //}
             //Someone won
          //else if (buffer blah blah check winner status equals win) 
          //{
          //printf("You won or lost!");
          //sleep (10);
          //exit(1);
          //}
          //else
          //{
              //status = buffer[0];
              //buffer at whatever location contains hit/miss
              //0 for miss
          //}
        }
      
        //wait until you receive input from server about other user
        //while(status == -1) 
        //{
          readStatus = read(sockfd, buffer, MAX_BUFF_LEN);
          if (readStatus > 0) 
          {
            printf("And the server says: %s",buffer);
            //Parse hit for hit or miss
            status = ParseHitPacket(buffer,1);
          }

        //}
        //Now display the board again to user
        displayBoard(0);
      //}
    //reset hit
    hit = -1;

    }

    free(input); // release memory allocated for user
  }
}

