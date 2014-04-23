/*
**HTTP1.0 server
**Supports GET and POST
**
**Using code from blog.abijeetr.com/2010/04/very-simple-http-server-written-in-c.html
**
**Created by Ana Diez
**Last modified: 4/13/14
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
//might not need this
#define MAXCONNECTIONS 1000
//#define BYTES 1024

#define DEF_PORT "3333"
#define MAXBUFLEN 1024

//might not need to specify the root
//char *ROOT;

//socket file descriptor
int sockfd;
int clients[MAXCONNECTIONS];

void setupServer(char* );
//handleRequests with int for multiple clients
void handleRequests(int clientNo);
int get(char*, int);
int post(char*, int);
void queryRun(int,char*,char*);
void interrupt(int sig);


int main(int argc, char* argv[]){

    struct sockaddr_in client;
    socklen_t addr_len;
    char port[6];

    int clientNo;

    if(signal(SIGINT,interrupt) == SIG_ERR){
        perror("could not set signal handler");
        exit(EXIT_FAILURE);
    }

    //assuming a good user. might parse later
    //setup port number
    memset(port,0,6);
    if(argc > 1){
        if(argc > 3){
            exit(EXIT_FAILURE);
        }
        if(getopt(argc,argv,"p:")){
            strcpy(port,optarg);
        }
    }else{
        strcpy(port,DEF_PORT);
    }


    clientNo = 0;
    int i;
    for(i = 0; i < MAXCONNECTIONS; i++){
        clients[i] = -1;
    }

    //start server before accepting connections
    setupServer(port);

    //accept connection (for multiple connections fork) 
    while(1){
        addr_len = sizeof(client);
        clients[clientNo] = accept(sockfd, (struct sockaddr*) &client, &addr_len);
        if(clients[clientNo] < 0 ){
            perror("error accepting client");
        }else{
            //if we're in the child process
            if(fork() == 0){
                handleRequests(clientNo);
                exit(EXIT_SUCCESS);
            }
        }
        while(clients[clientNo] !=-1){
            clientNo = (clientNo+1)%MAXCONNECTIONS;
        }
    }
}

void setupServer(char* port){
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
    if(listen(sockfd,MAXCONNECTIONS) != 0){
        perror("error listening for connectons");
        exit(EXIT_FAILURE);
    }

}

void handleRequests(int clientNo){
//takes int when multiple clients
//void handleRequests(int clientIndex);

    char buf[MAXBUFLEN];
    memset(buf,0,MAXBUFLEN);
    int numbytes;

//receive message with recv
    numbytes = recv(clients[clientNo],buf,MAXBUFLEN,0);

    if(numbytes <= 0){
        perror("recv error");
    }

//look at initial line, must start with GET or POST
    if(strncmp(buf,"GET",3) == 0){
    //get
        get(buf,clientNo);
    }else if(strncmp(buf,"POST",4) == 0){
    //post
        post(buf,clientNo);
    }else{
    //unsupported
    //send a bad request error
    write(clients[clientNo], "HTTP/1.0 400 Bad Request1\n", 25);
    exit(EXIT_FAILURE);
    }

//close socket and exit
    shutdown(clients[clientNo], SHUT_RDWR);
    close(clients[clientNo]);
    clients[clientNo] = -1;
}
//might change the return values of get and post
//from int to what's actually sent back - possibly char*
int get(char* buf, int clientNo){

//stores data from request, GET file HTTP/1.x
    char *request[3];
    char path[MAXBUFLEN];
    char query[MAXBUFLEN];
    FILE *fp;
    int bytes_read,eof;
    char data[MAXBUFLEN];

    printf("%s\n",buf);

//parse by lines
    request[0] = strtok(buf, " \t\n");
    int compare = strncmp(request[0], "GET",3);
    if(compare ==0){
        request[1] = strtok(NULL, " \t");
        request[2] = strtok(NULL, " \t\n");
        int i,j;
        i = strncmp(request[2],"HTTP/1.0",8);
        j = strncmp(request[2],"HTTP/1.1",8);
        if(i!=0 && j!=0){
            write(clients[clientNo], "HTTP/1.0 400 Bad Request1\n", 25);
        }else{
            compare = strncmp(request[1],"/",1);
            if(compare == 0){
                strcpy(request[1],"readme.txt");
            }
            memset(path,0,MAXBUFLEN);
            //copy the file into path
            strcpy(path,strtok(request[1]," ?\t\n"));
            memset(query,0,MAXBUFLEN);
            char* stuff;
            stuff = strtok(NULL,"\t\n");

            if(stuff != NULL){
                //not url unencoded.
                strcpy(query, stuff);
                queryRun(clientNo,path,query);
                memset(path,0,MAXBUFLEN);
                sprintf(path,"temp%d.txt",clientNo); 
            }
//send data
//send back contents
//use headers
//content type
//content length
            if(fp != NULL){
                fclose(fp);
                fp = NULL;
            }
            fp = fopen(path,"r");
            if(fp != NULL){
                send(clients[clientNo],"HTTP/1.0 200 OK\n",17,0);
                //add content type header
                char header[50];
                char* temp;
                strtok(path," .\n\t");
                temp = strtok(NULL, " \n\t\0");
                memset(header,0,50);
                sprintf(header,"Content type: %s\n",temp);
                send(clients[clientNo],header,strlen(header),0); 
 
                //add content length header
                int start=ftell(fp);
                fseek(fp,0L,SEEK_END);
                int size = ftell(fp);
                fseek(fp,start,SEEK_SET);
                memset(header,0,50);
                sprintf(header,"Content length: %d\n\n", size);
                send(clients[clientNo],header, strlen(header),0);
 
                memset(data,0,MAXBUFLEN);
                eof = 0;
                bytes_read = 0;
                while(eof == 0){
                    bytes_read = fread(data,sizeof(char),MAXBUFLEN,fp);
                    eof = feof(fp);
                    write(clients[clientNo],data,bytes_read);
                }
            }else{
                write(clients[clientNo],"HTTP/1.0 404 Not Found\n",23);
            }
        }
    }
//if file url is a pathname return 404 not found
//if file url is a query start program
  //send query through stdin
  //stream back stdout
    fclose(fp);
    fp = NULL;
    return;
}

int post(char* buf, int clientNo){
//parse 0-x headers
    char *request[3];
    char path[MAXBUFLEN], dataPath[MAXBUFLEN];
    char query[MAXBUFLEN];
    FILE *fp;
    int bytes_read,eof;
    char data[MAXBUFLEN];

    printf("%s\n",buf);

//parse by lines
//first line is POST file HTTP/1.0
    request[0] = strtok(buf, " \t\n");
    int compare = strncmp(request[0], "POST",4);
    if(compare ==0){
        request[1] = strtok(NULL, " \t");
        request[2] = strtok(NULL, " \t\n");
        int i,j;
        i = strncmp(request[2],"HTTP/1.0",8);
        j = strncmp(request[2],"HTTP/1.1",8);
        if(i!=0 && j!=0){
            write(clients[clientNo], "HTTP/1.0 400 Bad Request2\n", 25);
        }else{
          compare = strncmp(request[1],"/\0",2);
          if(compare == 0){
              write(clients[clientNo],"HTTP/1.0 400 Bad Request3\n",25);
          }else{
            //assuming query is in the message part of the request
            compare = 1;
            char* temp;
            memset(query,0,MAXBUFLEN);
            while(temp !=NULL){
                compare++;
                temp = strtok(NULL, "\n");

                if(temp!=NULL){
                    compare = strncmp("\r",temp,1);
                
                    if(compare == 0){
                        temp = strtok(NULL, "\r\n");
                        memset(query,0,MAXBUFLEN);
                        if(temp != NULL){
                            strcpy(query,temp);
                        }else{
                            break;
                        }
                    }
                }
            }
            //copy the file into path
            memset(path,0,MAXBUFLEN);
            strcpy(path,strtok(request[1]," ?\t\n"));
            queryRun(clientNo,path,query);
            //send stuff
            sprintf(dataPath,"temp%d.txt",clientNo);
            if(fp != NULL){
                fclose(fp);
                fp = NULL;
            }
            fp = fopen(dataPath,"r");
            if(fp != NULL){
                send(clients[clientNo],"HTTP/1.0 200 OK\n",17,0);
                //add content type header
                char header[50];
                memset(header,0,50);
                sprintf(header,"Content type: text\n");
                send(clients[clientNo],header,strlen(header),0); 
 
                //add content length header
                int start=ftell(fp);
                fseek(fp,0L,SEEK_END);
                int size = ftell(fp);
                fseek(fp,start,SEEK_SET);
                memset(header,0,50);
                sprintf(header,"Content length: %d\n\n", size);
                send(clients[clientNo],header, strlen(header),0);
 
                memset(data,0,MAXBUFLEN);
                eof = 0;
                bytes_read = 0;
                while(eof == 0){
                    bytes_read = fread(data,sizeof(char),MAXBUFLEN,fp);
                    eof = feof(fp);
                    write(clients[clientNo],data,bytes_read);
                }
            }else{
                write(clients[clientNo],"HTTP/1.0 404 Not Found\n",23);
            }
          }
        }
    }
}

//program running w/queries
//might add value for adding args
void queryRun(int n,char* program,char* query){
FILE* fp;
int died, status;
char toRun[MAXBUFLEN];
char filename[15];

memset(toRun,0,MAXBUFLEN);
strcpy(toRun,"/bin/");
strcat(toRun,program);
memset(filename,0,15);
sprintf(filename,"temp%d.txt",n);
//redirect stdout into file
   fp = freopen(filename,"w+",stdout);
//fork
  if(fork() == 0){
//redirect stdout into file
//   fp = freopen(filename,"w+",stdout);
//start program at program
    if(strlen(query) == 0){
        execl(toRun,program,(char *)NULL);
        return;
    }else{
        execl(toRun,program,query,(char *)NULL);
        return;
    }
//close file
  }else{
    died = wait(&status);
    fclose(fp);
    return;
  }
}

void interrupt(int sig){
    if(sig == SIGINT){
        printf("\nexiting the server\n");
        close(sockfd);
        exit(EXIT_SUCCESS);
    }
}
