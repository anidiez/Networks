#made by Ana Diez for the http server project
#last edited 4/18/14 @12:55

CC = gcc
LOCALHOST = localhost
REMOTEHOST = ec2-50-19-234-239.compute-1.amazonaws.com
PORT = 3335

all:
	${CC} -o http http-server.c ${LIBS}

help:
	@echo To run with the default port (3333) run default-http. 
	@echo To run with a specified port run port-http.
	@echo There is no driver (yet) so you need to pass in GET and POST requests from your own browser.

default-http:
	./http

port-http:
	./http -p ${PORT}

clean:
	-rm http
	-rm temp*
