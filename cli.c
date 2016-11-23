/*
Name       : Michael Bido-Chavez (euid: mb0501)
Due Date   : April 12, 2016
Class      : CSCE 3600
Instructor : Dr. Mark A. Thompson

Minor 7 server
----------------
Description:
  client to request returns and buy's from the server (buy's 80 percent of
  the time). each client will make a total of 15 requests. at the end, close
  connection and print out DB.
----------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/time.h>


int main(int argc, char *argv[])
{
  int theSock; // listen socket
  int n; // return from write and read
  int portNum; // port number
  char buffer[1024]; // buffer for messages
  struct sockaddr_in servAddr; // server address struct
  struct hostent *theServer; // server struct
  // check for proper num of args
  if (argc != 3) {
     fprintf(stderr,"usage: %s hostname port\n", argv[0]);
     exit(0);
  }
  // set port number
  portNum = atoi(argv[2]);
  // create socket, internet domain, socket stream, default
  theSock = socket(AF_INET, SOCK_STREAM, 0);
  if (theSock < 0) {
    fprintf(stderr,"ERROR: could not open socket.\n");
    exit(1);
  }
  // sets argv1 to theServer's name – returns a structure
  theServer = gethostbyname(argv[1]);
  if (theServer == NULL) {
    fprintf(stderr,"ERROR: could not set host.\n");
    exit(1);
  }
  // set values of server address struct, create address
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;
  servAddr.sin_port = htons(portNum);
  memset(servAddr.sin_zero, '\0', sizeof(servAddr.sin_zero));
  bcopy((char *)theServer->h_addr, (char *) &servAddr.sin_addr.s_addr, theServer->h_length);
  // connect to server
  if (connect(theSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
    fprintf(stderr,"ERROR: could not bind.\n");
    exit(1);
  }
  printf("Connected. Awaiting response from server...\n");
  // post connection, delcare new variables
  int i;
  char num[32];
  // while until number of requests reached
  while (strcmp(num,"0") != 0) {
    // clear buffer
    bzero(buffer, 1024);
    printf("[CLIENT]: ");
    scanf("%s", num);
    // copy buy into buffer
    strcpy(buffer, num);
    // write to server
    n = write(theSock, buffer,strlen(buffer));
    if (n < 0) {
      fprintf(stderr,"ERROR: could not write to socket.\n");
    }
    // clear buffer
    bzero(buffer, 1024);
    // read from server, then print
    n = read(theSock,buffer,1023);

    if (n < 0) {
      fprintf(stderr,"ERROR: could not read from socket.\n");
    } else {
      printf("[SERVER]: %s\n", buffer);
    }
  }
  //printf("Num of requests completed: %d\n", numRequests);
  close(theSock);
  return 0;
}
