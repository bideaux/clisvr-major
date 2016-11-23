/*
Group Code : 1
Name(s)    : Michael Bido-Chavez (euid: mb0501)
             Garrett Crowe (euid: gc0243)
             Sugn Mo Ahn (euid: sa0156)
             Miguel Martinez (euid: mam0755)
Due Date   : May 4th, 2016
Class      : CSCE 3600
Instructor : Dr. Mark A. Thompson

Major 2 client
----------------
Description:
A user will manually enter integer values at each client that will be sent to
the server. The server, in response to this integer value, will send the
client a current total so that the client is aware of the most up-to-date
“census” total. To quit, type 0
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
#include <arpa/inet.h>
#include <ifaddrs.h>


int main(int argc, char *argv[])
{
  int theSock, theCliSock/*, theCliSock2*/; // listen sockets
  int n, n1, n2; // return from write and read
  int cliSock/*, cliSock2*/; // sock for the client
  int servPortNum, cli1PortNum, cli2PortNum, clientNum; // input from command line
  float cpuUsageMax; // port numbers
  char buffer[1024], hostIP[64]; // buffer for messages
  char buffer2[512]; // second buffer for concurrency
  struct sockaddr_in servAddr, cliServAddr, cliAddr; // server address struct
  struct hostent *theServer, *theCliServer, *theCliServer2; // server struct
  char *address;
  socklen_t lenAddr; // for accepting new connections

  pid_t  pid;
  // check for proper num of args
  if (argc != 7) {
     fprintf(stderr,"usage: %s hostname serverport client1port client2port cpuUsageMax clientNum\n", argv[0]);
     exit(0);
  }
  // set port numbers
  servPortNum = atoi(argv[2]);
  cli1PortNum = atoi(argv[3]);
  cli2PortNum = atoi(argv[4]);
  // set cpu usage threshold
  cpuUsageMax = atoi(argv[5])/100;
  // get client number
  clientNum = atoi(argv[6]);
  // create socket, internet domain, socket stream, default
  // both for server and client-server
  theSock = socket(AF_INET, SOCK_STREAM, 0);
  theCliSock = socket(AF_INET, SOCK_STREAM, 0);
  //theCliSock2 = socket(AF_INET, SOCK_STREAM, 0);
  if (theSock < 0) {
    fprintf(stderr,"ERROR: could not open socket.\n");
    exit(1);
  }
  if (theCliSock < 0) {
    fprintf(stderr,"ERROR: could not open socket.\n");
    exit(1);
  }
  // if (theCliSock2 < 0) {
  //   fprintf(stderr,"ERROR: could not open socket.\n");
  //   exit(1);
  // }
  // sets argv1 to theServer's name – returns a structure

  // if (clientNum == 1) {
  //   printf("gethostname ip: %s\n", hostIP);
  //   theServer = gethostbyname(hostIP);
  // }
  // else {
  //   theServer = gethostbyname(argv[1]);
  // }
  // get host IP's
  theServer = gethostbyname(argv[1]);
  theCliServer = gethostbyname(argv[1]);
  //theCliServer2 = gethostbyname(argv[1]);
  if (theServer == NULL) {
    fprintf(stderr,"ERROR: could not set host.\n");
    exit(1);
  }
  if (theCliServer == NULL) {
    fprintf(stderr,"ERROR: could not set host.\n");
    exit(1);
  }
  // set values of server address struct, create address for server
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = INADDR_ANY;
  servAddr.sin_port = htons(servPortNum);
  memset(servAddr.sin_zero, '\0', sizeof(servAddr.sin_zero));
  bcopy((char *)theServer->h_addr, (char *) &servAddr.sin_addr.s_addr, theServer->h_length);
  // connect to server
  if (connect(theSock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
    fprintf(stderr,"ERROR: could not connect.\n");
    exit(1);
  }
  printf("Connected. Awaiting response from server...\n");
  // if client is 1, set up client-client connection. this client will act as a server as well
  if (clientNum == 1) {
    cliServAddr.sin_family = AF_INET;
    cliServAddr.sin_addr.s_addr = INADDR_ANY;
    cliServAddr.sin_port = htons(cli1PortNum);
    // set to 0
    memset(cliServAddr.sin_zero, '\0', sizeof(cliServAddr.sin_zero));
    // bind struct to socket, print error if error
    if (bind(theCliSock, (struct sockaddr *) &cliServAddr, sizeof(cliServAddr)) < 0) {
      fprintf(stderr,"ERROR: could not bind client-server.\n");
      exit(1);
    }
    printf("Awaiting client 2 connection... ");
    if (listen(theCliSock, 1) == 0) {
      //printf("Awaiting connections... ");
    } else {
      fprintf(stderr,"ERROR: could not listen.\n");
      exit(1);
    }
    // accept first connection, new socket
    lenAddr = sizeof(cliAddr);
    if ((cliSock = accept(theCliSock, (struct sockaddr *) &cliAddr, &lenAddr)) < 0) {
      fprintf(stderr,"ERROR: could not accept connection 1.\n");
      exit(1);
    } else {
      printf("Client 2 connected.\n");
    }
  }
  // else, if client 2, create second outgoing connection
  else {
    // set values of server address struct, create address
    cliServAddr.sin_family = AF_INET;
    cliServAddr.sin_addr.s_addr = INADDR_ANY;
    cliServAddr.sin_port = htons(cli1PortNum);
    memset(cliServAddr.sin_zero, '\0', sizeof(cliServAddr.sin_zero));
    bcopy((char *)theCliServer->h_addr, (char *) &cliServAddr.sin_addr.s_addr, theCliServer->h_length);
    // connect to server
    if (connect(theCliSock, (struct sockaddr *) &cliServAddr, sizeof(cliServAddr)) < 0) {
      fprintf(stderr,"ERROR: could not connect to server.\n");
      exit(1);
    }
  }
  // this was for the 2-way client server as above, but it created errors. Work on this
  /*
  if (clientNum != 1) {
    cliAddr.sin_family = AF_INET;
    cliAddr.sin_addr.s_addr = INADDR_ANY;
    cliAddr.sin_port = htons(cli2PortNum);
    // set to 0
    memset(cliAddr.sin_zero, '\0', sizeof(cliAddr.sin_zero));
    // bind struct to socket, print error if error
    if (bind(theCliSock2, (struct sockaddr *) &cliAddr, sizeof(cliAddr)) < 0) {
      fprintf(stderr,"ERROR: could not bind.\n");
      exit(1);
    }
    printf("Awaiting client 1 connection... ");
    if (listen(theCliSock2, 1) == 0) {
      //printf("Awaiting connections... ");
    } else {
      fprintf(stderr,"ERROR: could not listen.\n");
      exit(1);
    }
    // accept first connection, new socket
    lenAddr = sizeof(cliAddr);
    if ((cliSock2 = accept(theCliSock2, (struct sockaddr *) &cliAddr, &lenAddr)) < 0) {
      fprintf(stderr,"ERROR: could not accept connection 1.\n");
      exit(1);
    } else {
      printf("Client 1 connected.\n");
    }
  }
  else {
    sleep(1);
    // set values of server address struct, create address
    cliAddr.sin_family = AF_INET;
    cliAddr.sin_addr.s_addr = INADDR_ANY;
    cliAddr.sin_port = htons(cli2PortNum);
    memset(cliAddr.sin_zero, '\0', sizeof(cliAddr.sin_zero));
    bcopy((char *)theCliServer2->h_addr, (char *) &cliAddr.sin_addr.s_addr, theCliServer2->h_length);
    // connect to server
    if (connect(theCliSock2, (struct sockaddr *) &cliAddr, sizeof(cliAddr)) < 0) {
      fprintf(stderr,"ERROR: could not bind.\n");
      exit(1);
    }
  }
  */
  // post connection, delcare new variables
  FILE *fp; // file pointer for cpu usage
  int i; // counter
  int passed_val, total = 0; // total to return to clients
  char num[32], cliNum[32]; // for atoi conversion
  float time1[4], time2[4], cpuUse; // storing vals for cpu usage calculation
  fd_set fds; // file descriptors to poll
  // this was to fork a process to poll for client to client connection
  // in the occurance of one of the programs exceeding the cpu threshold and
  // needed to Disconnect. work on this
  if (/*clientNum == 1*/ 1!=1) {
  //   pid = fork();
  // }
  // if (pid == 0 && clientNum == 1) {
  //   n2 = read(theCliSock,buffer2, 511);
  //   printf("Connection Accepted\n");
  //   strncpy(num, buffer, 31);
  //   num[31] = '\0';
  //   passed_val = atoi(num);
  //   printf("Received %s from Other Client\n", num);
  //   printf("Client Disconnected\n");
  //   close(theCliSock);
   }
  else {
    // while input does not equal quit number
    while (strcmp(num,"0") != 0) {
      // clear buffer
      bzero(buffer, 1024);
      printf("[CLIENT] – Enter data to send: ");
      // get input
      scanf("%s", num);
      if (strcmp(num,"0") == 0) {
        break;
      }
      // calculate cpu usage afer input.
      fp = fopen("/proc/stat","r");
      fscanf(fp,"%*s %f %f %f %f",&time1[0],&time1[1],&time1[2],&time1[3]);
      fclose(fp);
      sleep(1);
      fp = fopen("/proc/stat","r");
      fscanf(fp,"%*s %f %f %f %f",&time2[0],&time2[1],&time2[2],&time2[3]);
      fclose(fp);
      /*
      cpu usage acquisition acquired from:
      http://stackoverflow.com/questions/3769405/determining-cpu-utilization
      http://www.linuxhowtos.org/System/procstat.htm

      cpu usage = cpu activity @ time 2 - cpu activity @ time 1 /
      cpu activity + idle amount @ time 2 - cpu activity + idle amount @ time 1

      part1 = (user+nice+system) processes @ time 2 minus (user+nice+system)
      processes @ time 1. part2 = (user+nice+system) processes + idle processor
      @ time 2 minus (user+nice+system) processes + idle processor @ time 1

      cpu usage % = part1 / part2
      */
      // copy buy into buffer

      cpuUse = ((time2[0]+time2[1]+time2[2]) - (time1[0]+time1[1]+time1[2])) /
        ((time2[0]+time2[1]+time2[2]+time2[3]) - (time1[0]+time1[1]+time1[2]+time1[3]));
      // print out message
      if (cpuUse > cpuUsageMax) {
        printf("CPU Utilization: %2.2f percent. threshold %2.2f percent exceeded\n", cpuUse, cpuUsageMax);
        // this would send the message to the other client and then quit.
        // if (clientNum != 1) {
        //   strcpy(buffer2, num);
        //   printf("Sending %s to CLIENT 1\n",num);
        //   n1 = write(theCliSock,buffer2,strlen(buffer2));
        //   printf("CLIENT Disconnecting...\n");
        // }
        // break;
      }
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
        fprintf(stderr,/*"ERROR: could not read from socket.\n*/" ");
        break;
      } else {
        // print out server return
        printf("[SERVER] – Total: %s\n", buffer);
      }
    }
    printf("[CLIENT] Disconnecting...\n");
  }
  // upon being done
  //printf("Num of requests completed: %d\n", numRequests);
  n = close(theSock);
  if (n < 0) {
    fprintf(stderr,/*"ERROR: could not read from socket.\n*/" ");
  }
  n = close(theCliSock);
  if (n < 0) {
    fprintf(stderr,/*"ERROR: could not read from socket.\n*/" ");
  }

  return 0;
}
