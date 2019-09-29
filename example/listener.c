#include "lib.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<signal.h>
#include<fcntl.h>

#define CONNMAX 1000
#define BYTES 1024

char *ROOT;
int listenfd;
void error(char *);
void startServer(char *);
void respond(int);

void startServer(char *port)
{
   struct addrinfo hints, *res, *p;

   memset (&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;
   if (getaddrinfo( NULL, port, &hints, &res) != 0)
   {
      perror ("getaddrinfo() error");
      exit(1);
   }
   
   for (p = res; p!=NULL; p=p->ai_next)
   {
      listenfd = socket (p->ai_family, p->ai_socktype, 0);
      if (listenfd == -1) continue;
      if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) break;
   }
   if (p==NULL)
   {
      perror ("socket() or bind()");
      exit(1);
   }

   freeaddrinfo(res);

   if ( listen (listenfd, 1000000) != 0 )
   {
      perror("listen() error");
      exit(1);
   }
}


void listener(node_parameter* param) {
   long long client;
   struct sockaddr_in clientaddr;
   socklen_t addrlen;
   char c;    

   char PORT[6];
   strcpy(PORT,"10000");

   printf("Server started at port no. %s%s%s with root directory as %s%s%s\n","\033[92m",PORT,"\033[0m","\033[92m",ROOT,"\033[0m");
   
   startServer(PORT);

   while (1)
   {
      addrlen = sizeof(clientaddr);
      client = (long long) accept (listenfd, (struct sockaddr *) &clientaddr, &addrlen);

      PUSH(param, client);
   }

   return 0;

}
