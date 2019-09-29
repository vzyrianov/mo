#include "lib.h"
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

#define BYTES 1024

char* ROOT;

void respond(node_parameter* param)
{
   ROOT = getenv("PWD");
   
   while(1) {
      long long client_long = TAKE(param);
      int client = (int) client_long;

      char mesg[99999], *reqline[3], data_to_send[BYTES], path[99999];
      int rcvd, fd, bytes_read;

      memset( (void*)mesg, (int)'\0', 99999 );

      rcvd=recv(client, mesg, 99999, 0);

      if (rcvd<0)    // receive error
         fprintf(stderr,("recv() error\n"));
      else if (rcvd==0)    // receive socket closed
         fprintf(stderr,"Client disconnected upexpectedly.\n");
      else    // message received
      {
         printf("%s", mesg);
         reqline[0] = strtok (mesg, " \t\n");
         if ( strncmp(reqline[0], "GET\0", 4)==0 )
         {
            reqline[1] = strtok (NULL, " \t");
            reqline[2] = strtok (NULL, " \t\n");
            if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
            {
               write(client, "HTTP/1.0 400 Bad Request\n", 25);
            }
            else
            {
               if ( strncmp(reqline[1], "/\0", 2)==0 )
                  reqline[1] = "/index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...

               strcpy(path, ROOT);
               strcpy(&path[strlen(ROOT)], reqline[1]);
               printf("file: %s\n", path);

               if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
               {
                  send(client, "HTTP/1.0 200 OK\n\n", 17, 0);
                  while ( (bytes_read=read(fd, data_to_send, BYTES))>0 )
                     write (client, data_to_send, bytes_read);
               }
               else    write(client, "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
            }
         }
      }

      shutdown (client, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
      close(client);
   }
}
