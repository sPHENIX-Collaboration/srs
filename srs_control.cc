
using namespace std;


#include <iostream>
#include <iomanip>


#include <parseargument.h>

#include "srs_utils.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include <string>

#define SRS_IP SRSIP

#ifndef SRS_IP
#define SRS_IP "10.0.0.3"
#endif

#define RECPORT 6007


void exithelp( const int ret =1)
{
  cout  << std::endl;
  cout << " srs_control usage " << std::endl;
  cout << "   srs_control info               show how we ae configured" << std::endl;
  cout << "   srs_control DAQ_IP  <ip>       set ip of DAQ stream" << std::endl;
  cout << "   srs_control BLCK_MODE  val     trigger mode" << std::endl;
  cout << "   srs_control BLCK_TRGBURST val  number of time slots (<=9)" << std::endl;
  cout << "   srs_control BLCK_FREQ  val     dead time or frequency" << std::endl;
  cout << "   srs_control EVBLD_CHMASK val   channel mask for hybrids" << std::endl;
  cout << "   srs_control EVBLD_DATALENGTH val  length of capture window" << std::endl;
  cout << "   srs_control RO_ENABLE          start acquisition" << std::endl;
  cout << "   srs_control RO_DISABLE         stop acquisition" << std::endl;
  cout << "   srs_control readfpga           read all fpga parameters" << std::endl;
  cout << "   srs_control readapv            read all APV parameters" << std::endl;
  cout << "   srs_control readadccard        read all ADC card parameters" << std::endl;
  cout << "   srs_control readhybrid         read all APV Hybrid card parameters" << std::endl;
  cout << "   srs_control GETVALUE port adr sadr      get a particular register" << std::endl;
  cout << "   srs_control SETVALUE port adr sadr val  set a particular register" << std::endl;
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


  std::string real_srs_ip;

  if ( getenv("SRSIP")  )
    {
      real_srs_ip = getenv("SRSIP");
    }
  else
    {
      real_srs_ip = SRS_IP;
    }

  returnstructure returnblock;
  
  unsigned int val = 0;
  unsigned int subaddress;


  if ( argc > 2 )
    {
      int i;

  
      if ( strcmp( argv[1], "readhybrid") == 0)  
	{
	  if ( argc < 4) exithelp();

	  val = get_uvalue( argv[2]);
	  if ( val >7) exithelp();

	  unsigned int reg  = get_uvalue( argv[3]);

	  unsigned int s[] = { 0x800, 0x400, 0x200, 0x100, 0x8000, 0x4000, 0x2000, 0x1000};

	  subaddress = ( s[val] ) + 1;   // we hardcode "Master APV" for now
	  reqport=6263;

	  cout << hex << "subaddress " << subaddress << endl;


	  int r = read_parameter ( real_srs_ip.c_str(), reqport, reg,  &returnblock, subaddress);

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

      if ( strcmp( argv[1], "DAQ_IP") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  i = set_value(real_srs_ip.c_str(), 10, 0, 6007, val);
	}

      else if ( strcmp( argv[1], "BLCK_MODE") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  i = set_value( real_srs_ip.c_str(), 0, 0, 6039, val);
	}

      else if ( strcmp( argv[1], "BLCK_TRGBURST") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  i = set_value( real_srs_ip.c_str(), 1, 0, 6039, val);
	}

      else if ( strcmp( argv[1], "BLCK_FREQ") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  i = set_value( real_srs_ip.c_str(), 2, 0, 6039, val);
	}

      else if ( strcmp( argv[1], "EVBLD_CHMASK") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  val &=0xffff;
	  i = set_value( real_srs_ip.c_str(), 8, 0, 6039, val);
	}

      else if ( strcmp( argv[1], "EVBLD_DATALENGTH") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  val &=0xffff;
	  i = set_value( real_srs_ip.c_str(), 9, 0, 6039, val);
	}

      else if ( strcmp( argv[1], "HYBRID_APVLATENCY") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  
	  i = set_value( real_srs_ip.c_str(), 2, 0xff01, 6263, val);
	}

      else if ( strcmp( argv[1], "SETVALUE") == 0)  
	{
	  if ( argc < 6 ) exithelp(1);

	  int port = get_uvalue( argv[2]);
	  int adr  = get_uvalue( argv[3]);
	  int sadr = get_uvalue( argv[4]);
	  val      = get_uvalue( argv[5]);

	  i = set_value( real_srs_ip.c_str(), adr, sadr, port, val);
	}

      else if ( strcmp( argv[1], "GETVALUE") == 0)  
	{
	  if ( argc < 5 ) exithelp(1);
	  
	  int port = get_uvalue( argv[2]);
	  int adr  = get_uvalue( argv[3]);
	  int sadr = get_uvalue( argv[4]);
	  
	  int r = read_parameter ( real_srs_ip.c_str(), port, adr,  &returnblock, sadr);
	  
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
  else
    {

      int i;

      if ( strcmp( argv[1], "info") == 0)  
	{
	  cout << " Default SRS IP Address: " <<  real_srs_ip << endl;
	}

      else if ( strcmp( argv[1], "help") == 0)  
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
	  i = set_value( real_srs_ip.c_str(), 0xF, 0, 6039, val);
	}


      else if ( strcmp( argv[1], "RO_DISABLE") == 0)  
	{
	  val = 0;
	  set_value( real_srs_ip.c_str(), 0xF, 0, 6039, val);
	}


      if ( reqport && regcount )
	{
	  int r = read_params ( real_srs_ip.c_str(), reqport, regcount,  &returnblock);

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

