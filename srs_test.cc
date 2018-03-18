#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <iomanip>
#include <fcntl.h>


#define BUFLEN 4096
#define NPACK 100
#define PORT 6006

using namespace std;


void diep(char *s)
{
  perror(s);
  exit(1);
}


main(int argc, char *argv[])
{

  if ( argc < 2)
    {
      cout << "Usage: " << argv[0] << " outfile" << endl;
      exit(1);
    }


  struct sockaddr_in si_me, si_other;
  int s, i;
  socklen_t slen=sizeof(si_other);
  int count=0;
  
  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    diep("socket");

  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(s, (sockaddr *)&si_me, sizeof(si_me))==-1)
    diep("bind");

  int d[5000]; 
  int r;

  count = 100;

  int sleeptime = 0;

  int ifd =  open(argv[1], O_WRONLY | O_CREAT | O_EXCL | O_LARGEFILE , 
		  S_IRWXU | S_IROTH | S_IRGRP );
  if (ifd < 0) 
    {
      cerr << "file exists, I will not overwrite " << argv[1] << endl;
      exit(1);
    }

  const unsigned int eop_marker = 0xf000f000;

  for (i = 0; i< count; i++)
    {
      if ( ( r = recvfrom(s, d, 4*5000, 0, (sockaddr *) &si_other, &slen) ) ==-1)
	diep("recvfrom()");

      cout << "length received: " << r << endl;

      write( ifd, d, r);
      write( ifd, &eop_marker, 4);
    } 


  close(s);
  close (ifd);
  return 0;
}
