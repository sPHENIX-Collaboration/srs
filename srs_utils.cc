
#include "srs_utils.h"

#include <iostream>
#include <stdio.h>

#include <arpa/inet.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#define RECPORT 6007

using namespace std;

int read_params ( const char *ip, const int regport, const int regcount,  returnstructure *returnblock, const int subaddress)
{
  struct sockaddr_in _server;
  int _sockfd;
  in_addr _srsip;

  if (inet_aton (ip, &_srsip)==0)
    {
      cerr << "invalid IP address " << ip << endl;
      return 1;
    }

  socklen_t slen=sizeof(_server);

  if ( ( _sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) ==-1)
    {
      perror("socket " );
      return -1;
    }

  struct sockaddr_in si_return;
  socklen_t sl = sizeof(si_return );

  memset((char *) &si_return, 0, sizeof(si_return));
  si_return.sin_family = AF_INET;
  si_return.sin_port = htons(RECPORT);
  si_return.sin_addr.s_addr = htonl(INADDR_ANY);


  if (bind(_sockfd, (sockaddr *)&si_return, sizeof(si_return))==-1)
    {
      perror("bind");
      return -6;
    }


  memset ( (char *) &_server, 0, slen);
  _server.sin_family  = AF_INET;
  _server.sin_addr = _srsip;
  _server.sin_port = htons(regport);

  requeststructure request;

  request.RequestId = ntohl(0x87654000); 
  request.SubAddress =ntohl(subaddress);
  request.CmdField1 = ntohl(0xBBAAFFFF); 
  request.CmdField2 = 0;

  for ( int i = 0; i < regcount; i++) 
    {
      request.data[i] = htonl(i);
    }

  if (sendto(_sockfd, &request, 4*4+regcount*4, 0, (sockaddr *) &_server, slen)==-1)
    {
      perror("sendto  " );
      return -2;
    }

  int r;

  
  fd_set read_flag;
  
  FD_ZERO(&read_flag);
  FD_SET(_sockfd, &read_flag);

  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 300000;

  int retval = select(_sockfd+1, &read_flag, NULL, NULL, &tv);
  if ( retval >0) 
    {

      if ( ( r = recvfrom(_sockfd, returnblock, sizeof(*returnblock), 0, (sockaddr *) &si_return, &sl) ) ==-1)
	{
	  perror("recfrom");
	  return -5;
	}

      close (_sockfd);
      return r;
    }
  else
    {
      cout << "timeout " << endl;
      close ( _sockfd);
      return -1;
    }


  return 0;

}

int read_parameter ( const char *ip, const int regport, const unsigned int adr,  returnstructure *returnblock, const int subaddress)
{
  struct sockaddr_in _server;
  int _sockfd;
  in_addr _srsip;

  if (inet_aton(ip, &_srsip)==0)
    {
      cerr << "invalid IP address " << ip << endl;
      return 1;
    }



  socklen_t slen=sizeof(_server);

  if ( ( _sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) ==-1)
    {
      perror("socket " );
      return -1;
    }

  struct sockaddr_in si_return;
  socklen_t sl = sizeof(si_return );

  memset((char *) &si_return, 0, sizeof(si_return));
  si_return.sin_family = AF_INET;
  si_return.sin_port = htons(RECPORT);
  si_return.sin_addr.s_addr = htonl(INADDR_ANY);


  if (bind(_sockfd, (sockaddr *)&si_return, sizeof(si_return))==-1)
    {
      perror("bind");
      return -6;
    }


  memset ( (char *) &_server, 0, slen);
  _server.sin_family  = AF_INET;
  _server.sin_addr = _srsip;
  _server.sin_port = htons(regport);

  requeststructure request;

  request.RequestId = ntohl(0x87654000); 
  request.SubAddress =ntohl(subaddress);
  request.CmdField1 = ntohl(0xBBAAFFFF); 
  request.CmdField2 = 0;

  request.data[0] = htonl(adr);


  if (sendto(_sockfd, &request, 4*4+4, 0, (sockaddr *) &_server, slen)==-1)
    {
      perror("sendto  " );
      close (_sockfd);
      return -2;
    }

  int r;

  
  fd_set read_flag;
  
  FD_ZERO(&read_flag);
  FD_SET(_sockfd, &read_flag);

  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 300000;

  int retval = select(_sockfd+1, &read_flag, NULL, NULL, &tv);
  if ( retval >0) 
    {

      if ( ( r = recvfrom(_sockfd, returnblock, sizeof(*returnblock), 0, (sockaddr *) &si_return, &sl) ) ==-1)
	{
	  perror("recfrom");
	  return -5;
	}

      close (_sockfd);
      return r;
    }
  else
    {
      cout << "timeout " << endl;
      close ( _sockfd);
      return -1;
    }


  return 0;

}




int set_value( const char *ip, const unsigned int adr, const unsigned int subaddress, const int reqport, const unsigned int val)
{

  struct sockaddr_in _server;
  int _sockfd;
  in_addr _srsip;
  in_addr _myip;

  if (inet_aton(ip, &_srsip)==0)
    {
      cerr << "invalid IP address " << ip << endl;
      return 1;
    }

  socklen_t slen=sizeof(_server);

  if ( ( _sockfd=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) ) ==-1)
    {
      perror("socket " );
      return -1;
    }

  struct sockaddr_in si_return;
  socklen_t sl = sizeof(si_return );

  memset((char *) &si_return, 0, sizeof(si_return));
  si_return.sin_family = AF_INET;
  si_return.sin_port = htons(RECPORT);
  si_return.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(_sockfd, (sockaddr *)&si_return, sizeof(si_return))==-1)
    {
      perror("bind");
      return -6;
    }


  memset ( (char *) &_server, 0, slen);
  _server.sin_family  = AF_INET;
  _server.sin_addr = _srsip;
  _server.sin_port = htons(reqport);

  requeststructure request;

  request.RequestId = htonl(0x87654000); 
  request.SubAddress =htonl(subaddress);
  request.CmdField1 = htonl(0xAAAAFFFF); 
  request.CmdField2 = 0;
  request.data[0] = htonl(adr);
  request.data[1] = htonl(val);
  

  if (sendto(_sockfd, &request, 4*4 + 2*4, 0, (sockaddr *) &_server, slen)==-1)
    {
      perror("sendto  " );
      return -2;
    }

  int r;

  
  fd_set read_flag;
  
  FD_ZERO(&read_flag);
  FD_SET(_sockfd, &read_flag);

  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 300000;


  returnstructure returnblock;

  int retval = select(_sockfd+1, &read_flag, NULL, NULL, &tv);
  if ( retval >0) 
    {

      if ( ( r = recvfrom(_sockfd, &returnblock, sizeof(returnblock), 0, (sockaddr *) &si_return, &sl) ) ==-1)
	{
	  perror("recfrom");
	  return -5;
	}

      close (_sockfd);
      return r;
    }
  else
    {
      cout << "timeout " << endl;
      close ( _sockfd);
      return -1;
    }


  return 0;

}
