//Creators Ana Diez and Steven Basart

//Libraries to include
#include "battleship.h"

//--Predefined
#define TRUE 1
#define FALSE 0
#define GAMEBOARD 100

//Declare the functions
//void
int powerN(int, int, int);
int convertPortToInt(int);
int setupGame(int);
int ParseInput(int,char* input);
char* getUserInput();
void displayBoard();
int CheckBounds(int location, int length, int oriented);
int CheckCollision(int location, int length, int oriented);


//Globals
char shipArray[GAMEBOARD];
int ships[5];


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
  int port = 0, sockfd;
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

char* getUserInput() 
{
  int max = 20;
  char* name = (char*)malloc(max); // allocate buffer
  if (name == 0) {exit(0);}

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
      name[i] = '\0';
      break;
    }
    name[i] = c;
    if (i==max-1) // buffer full
    { 
      max = max+max;
      name = (char*)realloc(name,max); // get a new and larger buffer
      if (name == 0) {exit(0);}
    }
    i++;
  }

  printf("The input is %s\n", name);
  return name;

}

void displayBoard()
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
      printf(" [%c]",shipArray[index*10+index2]);
      //We can split the print statements to print out X's were hit

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

  //if going to the right
  if (oriented == 1)
  {
    //11 because 10 can exist
    if((length + tens) > 11 ) 
    {
      printf("Sorry ship out of bounds. Try again\n");
      return (-1);
    }
  } 
  //if going down
  if(oriented == 0) 
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
  return (1);
}

//down == 0
void PlaceShip(char shippie, int location, int size, int oriented)
{
  int index = 0;

  //if going to the right
  if (oriented == 1)
  {
    for(index = 0; index < size; index++) 
    {
      shipArray[location + index] = shippie;
    }
  } 
  //if going down
  if(oriented == 0) 
  {
    for(index = 0; index < size; index++) 
    {
      shipArray[location + (10*index)] = shippie;
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

//Note to self pass in sockfd to write to server in case of good input*******
int ParseInput(int sockfd, char* input)
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
  printf("ship = %c orient = %c loc = %d",ship, orientation, location);
  fflush(stdout);
  #endif


  if(index < 3)
  {
    printf("index was less than three \n");
    return(-1);
  }

  if (orientation == 'd' || orientation == 'D')
  {
    oriented = 0;
  }
  else if (orientation == 'r' || orientation == 'R')
  {
    oriented = 1;
  }
  else 
  {
    printf("Invalid orientation. Only right or down.\n");
    return(-1);
  }

  #ifdef debug
  printf("Am I passed orientation?\n");
  fflush(stdout);
  #endif



  switch (ship) 
  {
    case 'A':
    case 'a':
      if(CheckAlreadyUsed(A) < 0)
      {return (-1);}
      if(CheckBounds(location, 5, oriented) < 0)
      {return (-1);}
      if(CheckCollision(location, 5, oriented) < 0)
      {return(-1);}
      PlaceShip(ship, location, 5, oriented);
      //write it back to server***********************************8i
      sprintf(string,"25%d;%d",location,oriented);
      write(sockfd,string,10);
      break;
    case 'B':
    case 'b':
      if(CheckAlreadyUsed(B) < 0)
      {return (-1);}
      if(CheckBounds(location, 4, oriented) < 0)
      {return (-1);}
      if(CheckCollision(location, 4, oriented) < 0)
      {return(-1);}
      PlaceShip(ship, location, 4, oriented);
      //write it back to server
      sprintf(string,"24%d;%d",location,oriented);
      write(sockfd,string,10);
   
      break;
    case 'C':
    case 'c':
      if(CheckAlreadyUsed(C) < 0)
      {return (-1);}
      if(CheckBounds(location, 3, oriented) < 0)
      {return (-1);}
      if(CheckCollision(location, 3, oriented) < 0)
      {return(-1);}
      PlaceShip(ship, location, 3, oriented);
      //write it back to server
      sprintf(string,"23%d;%d",location,oriented);
      write(sockfd,string,10);
 
      break;
    case 'S':
    case 's':
      if(CheckAlreadyUsed(S) < 0)
      {return (-1);}
      if(CheckBounds(location, 3, oriented) < 0)
      {return (-1);}
      if(CheckCollision(location, 3, oriented) < 0)
      {return(-1);}
      PlaceShip(ship, location, 3, oriented);
      //write it back to server
      sprintf(string,"23%d;%d",location,oriented);
      write(sockfd,string,10);
 
      break;
    case 'P':
    case 'p':
      if(CheckAlreadyUsed(P) < 0)
      {return (-1);}
      if(CheckBounds(location, 2, oriented) < 0)
      {return (-1);}
      if(CheckCollision(location, 2, oriented) < 0)
      {return(-1);}
      PlaceShip(ship, location, 2, oriented);
      //write it back to server
      sprintf(string,"22%d;%d",location,oriented);
      write(sockfd,string,10);
 
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
  int index = 0, readStatus = 0, placed = 0;
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

    printf("Select boat, location, and orientation(down or right)\n");
    printf("Example: \"B, C5, down\"\n");
    fflush(stdout);
    displayBoard();

    input = getUserInput();
    #ifdef debug
    printf("Did I survive getting input?\n");
    printf("And the input is %s\n",input);
    fflush(stdout);
    #endif

    //Parses the input to see if valid and avoid collisions
    index = ParseInput(sockfd, input);

    #ifdef debug
    printf("Did I parse?\n");
    fflush(stdout);
    #endif

    //For now since

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
 
}

