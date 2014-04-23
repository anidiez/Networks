Makefile for Battleships!
#last edited 4/19/14

CC = gcc
LOCALHOST = localhost
REMOTEHOST = ec2-50-19-234-239.compute-1.amazonaws.com
PORT = 3335

all:
	${CC} -o server battleship-server.c ${LIBS}
	${CC} -o client battleship-client.c ${LIBS}

help:
	@echo write help here. 


server:
	./server

client:
	./client

clean:
	-rm server
	-rm client
