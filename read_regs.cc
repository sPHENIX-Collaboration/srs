
using namespace std;


#include <iostream>
#include <iomanip>


#include <parseargument.h>



#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>


#define MY_IP "10.0.0.3"
#define SRS_IP "10.0.0.2"
#define RECPORT 6007


typedef struct {

  unsigned int RequestId;           
  unsigned int SubAddress;           
  unsigned int CmdField1;
  unsigned int CmdField2;
  unsigned int data[8896];

}  requeststructure;               


struct valuepair {
  unsigned int errorcode;           
  unsigned int value;
};

typedef struct {

  unsigned int RequestId;           
  unsigned int SubAddress;           
  unsigned int CmdField1;
  unsigned int CmdField2;
  struct valuepair vp[512];
}  returnstructure;               


int read_params ( const int regport, const int regcount,  returnstructure *returnblock);
int set_value( const unsigned int adr, const int reqport, const unsigned int val);

void exithelp( const int ret =1)
{
  cout  << std::endl;
  cout << " srs_control usage " << std::endl;
  cout << "   srs_control DAQ_IP  <ip>       set ip of DAQ stream" << std::endl;
  cout << "   srs_control BLCK_MODE  val     trigger mode" << std::endl;
  cout << "   srs_control BLCK_TRGBURST val  number of time slots (<=9)" << std::endl;
  cout << "   srs_control BLCK_FREQ  val     dead time or frequency" << std::endl;
  cout << "   srs_control EVBLD_CHMASK val   channel mask for hybrids" << std::endl;
  cout << "   srs_control RO_ENABLE          start acquisition" << std::endl;
  cout << "   srs_control RO_DISABLE         stop acquisition" << std::endl;
  cout << "   srs_control readfpga           read all fpga parameters" << std::endl;
  cout << "   srs_control readapv            read all APV parameters" << std::endl;
  cout << "   srs_control readadccard        read all ADC card parameters" << std::endl;
  cout << "    " << std::endl;
  exit(ret);
}

int main(int argc, char *argv[])
{
  
  int reqport = 0;
  int regcount =0;


  if ( argc==1) 
    {
      cout << "usage: " << argv[0] << " action " << endl;

      exithelp();
    }



  returnstructure returnblock;
  
  unsigned int val = 0;

  if ( argc > 2 )
    {
      int i;

      if ( strcmp( argv[1], "DAQ_IP") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  i = set_value( 10, 6007, val);
	}

      else if ( strcmp( argv[1], "BLCK_MODE") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  i = set_value( 0, 6039, val);
	}

      else if ( strcmp( argv[1], "BLCK_TRGBURST") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  i = set_value( 1, 6039, val);
	}

      else if ( strcmp( argv[1], "BLCK_FREQ") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  i = set_value( 2, 6039, val);
	}

      else if ( strcmp( argv[1], "EVBLD_CHMASK") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  val &=0xffff;
	  i = set_value( 8, 6039, val);
	}

    }
  else
    {

      int i;

      if ( strcmp( argv[1], "help") == 0)  
	{
	  exithelp(0);
	}

      else if ( strcmp( argv[1], "readfpga") == 0)  
	{
	  reqport = 6007;
	  regcount = 11;
	}

      else if ( strcmp( argv[1], "readapv") == 0)  
	{
	  reqport = 6039;
	  regcount = 21;
	}

      else if ( strcmp( argv[1], "readadccard") == 0)  
	{
	  reqport = 6519;
	  regcount = 6;
	}

      else if ( strcmp( argv[1], "RO_ENABLE") == 0)  
	{
	  val = 1;
	  i = set_value( 0xF, 6039, val);
	}


      else if ( strcmp( argv[1], "RO_DISABLE") == 0)  
	{
	  val = 0;
	  set_value( 0xF, 6039, val);
	}


      if ( reqport && regcount )
	{
	  int r = read_params ( reqport, regcount,  &returnblock);

	  cout << "received: " << r << "  bytes"  << endl;
	  if ( r>0)
	    {
	      cout << " Requestid: " << hex << ntohl(returnblock.RequestId) << dec << endl;
	      cout << " SubAdress: " << hex << ntohl(returnblock.SubAddress) << dec << endl;
	      cout << " CmdField1: " << hex << ntohl(returnblock.CmdField1) << dec << endl;
	      cout << " CmdField2: " << hex << ntohl(returnblock.CmdField2) << dec << endl;

	      int x = (r/4 -4)/2;
	      for ( int i = 0; i < x; i++)
		{
		  cout << setw(3) << i << "  " << setw(10) << hex << ntohl(returnblock.vp[i].value) <<dec << "  "; 
		  if ( returnblock.vp[i].errorcode) 
		    {
		      cout << setw(10) << hex << ntohl(returnblock.vp[i].errorcode) <<dec; 
		    }
		  else
		    {
		      cout << "  ok";
		    }
	      
		  cout << endl;
		}
	    }
	}
    }
  
  return 0;
}



int read_params ( const int regport, const int regcount,  returnstructure *returnblock)
{
  struct sockaddr_in _server;
  int _sockfd;
  in_addr _srsip;
  in_addr _myip;

  if (inet_aton(SRS_IP, &_srsip)==0)
    {
      cerr << "invalid IP address " << SRS_IP << endl;
      return 1;
    }

  if (inet_aton(MY_IP, &_myip)==0)
    {
      cerr << "invalid IP address " << SRS_IP << endl;
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
  request.SubAddress =ntohl(0x00000000);
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



int set_value( const unsigned int adr, const int reqport, const unsigned int val)
{

  struct sockaddr_in _server;
  int _sockfd;
  in_addr _srsip;
  in_addr _myip;

  if (inet_aton(SRS_IP, &_srsip)==0)
    {
      cerr << "invalid IP address " << SRS_IP << endl;
      return 1;
    }

  if (inet_aton(MY_IP, &_myip)==0)
    {
      cerr << "invalid IP address " << SRS_IP << endl;
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
  si_return.sin_addr = _myip;

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
  request.SubAddress =htonl(0x00000000);
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
