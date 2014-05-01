#Makefile for Battleships!
#last edited 4/19/14

CC = gcc
LOCALHOST = localhost
REMOTEHOST = ec2-50-19-234-239.compute-1.amazonaws.com
PORT = 3335

all:
	$(CC) -o server battleship-server.c -g
	$(CC) -o client battleship-client.c -g

#made "debug" a macro add -Ddebug on client to see lots of comments
debug:
	$(CC) -o server battleship-server.c -g -Ddebug
	$(CC) -o client battleship-client.c -g -Ddebug

help:
	@echo To run start server (i.e. ./server)
	@echo Then start two clients (i.e. ./client)


server:
	$(CC) battleship-server.c -o server

client:
	$(CC) battleship-client.c -o client

clean:
	-rm server
	-rm client
