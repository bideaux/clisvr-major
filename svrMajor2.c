/*
Group Code : 1
Name(s)    : Michael Bido-Chavez (euid: mb0501)
             Garrett Crowe (euid: gc0243)
             Sugn Mo Ahn (euid: sa0156)
             Miguel Martinez (euid: mam0755)
Due Date   : May 4th, 2016
Class      : CSCE 3600
Instructor : Dr. Mark A. Thompson

Major 2 server
----------------
Description:
The server will receive integer values from either of the two clients (in any
order) and add these values to its current total, which the server then
returns back to the originating client so that the client is aware of the most
up-to-date “census” total.
----------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

int main(int argc, char *argv[])
{
  int theSock; // listen socket
  int sock1; // socket for first connection
  int sock2; // socket for second connection
  int portNum; // port number
  char buffer[1024]; // buffer for messages
  struct sockaddr_in servAddr, cliAddr; // server and client structs
  socklen_t lenAddr; // for accepting new connections
  // check args
  if (argc != 2) {
    printf("usage: %s port\n", argv[0]);
    fprintf(stderr,"ERROR: no port provided, or improper usage.\n");
    exit(1);
  }

  // create socket, internet domain, socket stream, default
  theSock = socket(AF_INET, SOCK_STREAM, 0);
  if (theSock < 0) {
    fprintf(stderr,"ERROR: could not open socket.\n");
    exit(1);
  }
  // set port number
  portNum = atoi(argv[1]);
  // set values of server address struct, create address
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;
  servAddr.sin_port = htons(portNum);
  // set to 0
  memset(servAddr.sin_zero, '\0', sizeof(servAddr.sin_zero));
  // bind struct to socket, print error if error
  if (bind(theSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
    fprintf(stderr,"ERROR: could not bind.\n");
    exit(1);
  }
  // listen with 3 connections max, else print error
 printf("Awaiting connections... ");
 if (listen(theSock, 3) == 0) {
   //printf("Awaiting connections... ");
 } else {
   fprintf(stderr,"ERROR: could not listen.\n");
   exit(1);
 }
 // accept first connection, new socket
 lenAddr = sizeof(cliAddr);
 if ((sock1 = accept(theSock, (struct sockaddr *) &cliAddr, &lenAddr)) < 0) {
   fprintf(stderr,"ERROR: could not accept connection 1.\n");
   exit(1);
 } else {
   printf("Client 1 connected. ");
 }
 // accpet second connection, new socket
 if ((sock2 = accept(theSock, (struct sockaddr *) &cliAddr, &lenAddr)) < 0) {
   fprintf(stderr,"ERROR: could not accept connection 2.\n");
   exit(1);
 } else {
   printf("Client 2 connected. ");
 }
 printf("\nBoth clients connected.\n");
 // new variables
 int passed_val, total = 0; // total to return to clients
 int i; // increment
 int n1, n2; // returns from reading and writing
 int maxfd = (sock1 > sock2 ? sock1 : sock2) + 1; // for polling
 int sockReady;
 int c1 = 0, c2 = 0; // if the sockets have closed, bool
 char num[32];
 fd_set fds; // file descriptors to poll
 printf("Client 1 Handler Assigned\n");
 printf("Client 2 Handler Assigned\n");
 while (1) {
   if (c1 == 1 && c2 == 1) {
     break;
   }
   // Set up polling using select, from example doc
   FD_ZERO(&fds);
   FD_SET(sock1,&fds);
   //printf("Client 1 Handler Assigned\n");
   FD_SET(sock2,&fds);
   // wait for some input, from example doc
   sockReady = select(maxfd, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval*) 0);
   // if from first socket/client connection
   if (FD_ISSET(sock1, &fds) && c1 == 0) {
     //printf("Got Here 1\n");
     bzero(buffer, 1024);
     n1 = read(sock1,buffer, 1023);
     //printf("Got Here 2\n");
     if (n1 < 0) {
       fprintf(stderr,"ERROR: could no read from 1st socket\n");
     }
     // input from client, atoi and add to total.
     strncpy(num, buffer, 31);
     num[31] = '\0';
     passed_val = atoi(num);
     total += passed_val;
     if (passed_val != 0) {
       printf("[CLIENT 1]: %s\n",buffer);
     }

     //printf("this is parse: %s.",parse);
     memset(buffer, '\0', sizeof(buffer));
     // copy to buffer
     snprintf(buffer, 1023,"%d",total);
     // write to client
     n1 = write(sock1,buffer, 100);
     // close if sock is done
     if (passed_val == 0) {
       close(sock1);
       c1 = 1;
       printf("Client 1 closed connection.\n");
     }
     //printf("Got Here 3\n");
     if (n1 == -1 && c1 == 0) {
       fprintf(stderr,"[SERVER X]: Client 1 connection closed, cannot be reached\n");
       close(sock1);
       c1 = 1;
     }
     else if (n1 < 0) {
       fprintf(stderr,"ERROR: could not write to Client 1");
     }
   }
   // if from second socket/client connection
   if (FD_ISSET(sock2, &fds) && c2 == 0) {
     bzero(buffer, 1024);
     n2 = read(sock2,buffer, 1023);
     //printf("Got Here 2\n");
     if (n2 < 0) {
       fprintf(stderr,"ERROR: could no read from 2nd socket\n");
     }
     // input from client, atoi and output
     strncpy(num, buffer, 31);
     num[31] = '\0';
     passed_val = atoi(num);
     total += passed_val;
     if (passed_val != 0) {
       printf("[CLIENT 2]: %s\n",buffer);
     }
     //printf("this is parse: %s.",parse);
     memset(buffer, '\0', sizeof(buffer));
     // copy ticket to buffer
     snprintf(buffer, 1023,"%d",total);
     // write to client
     n2 = write(sock2,buffer, 100);
     // if user input is 0, close socket
     if (passed_val == 0) {
       close(sock2);
       printf("Client 2 closed connection.\n");
       c2 = 1;
     }
     //printf("Got Here 3\n");
     if (n2 == -1 && c2 == 0) {
       fprintf(stderr,"[SERVER X]: Client 2 connection closed, cannot be reached\n");
       close(sock2);
       c2 = 1;
     }
     else if (n1 < 0) {
       fprintf(stderr,"ERROR: could not write to Client 2");
     }
   }
   //printf("Got Here 1\n");
   //printf("c1 count: %d, c2 count: %d\n", s1, s2);
 }

 //printf("Got Here 4\n");
 // close the socket
 close(theSock);

 //printf("Final total: %d!\n", total);

 return  0;
}
