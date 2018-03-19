
#include <iostream>
#include <sstream>
#include <tspm_udp_communication.h>
#include <parseargument.h>

using namespace std;

#define MY_IP SRSIP

#ifndef MY_IP
#define MY_IP "10.0.0.2"
#endif



void exithelp( const int ret =1)
{

  cout  << std::endl;
  cout << " tspm_client usage " << std::endl;
  cout << "   tspm_client 0         read register 0" << std::endl;
  cout << "   tspm_client 0  4      write 4 into register 0" << std::endl;
  cout << "   tspm_client set-asic-disable-mask value    " << std::endl;
  cout << "   tspm_client disable-all-asics    " << std::endl;
  cout << "   tspm_client set-all-dacs lowvalue (0-0xfff) highvalue (0-0xfff) " << std::endl;
  cout << "   tspm_client udpheader1  [value]  read or write the header 1 value (reg 9)" << std::endl;
  cout << "   tspm_client udpheader2  [value]  read or write the header 2 value (reg 10)" << std::endl;
  cout << "   tspm_client reset-spi <towerboard>  reset the SPI data for a given tower" << std::endl;
  cout << "   tspm_client reset                full board reset" << std::endl;
  cout << "    " << std::endl;
  exit(ret);
}

main(int argc, char *argv[])
{

  if ( argc==1) 
    {
      cout << "usage: " << argv[0] << " register [value],  or:" << endl;

      exit(1);
    }

  tspm_udp_communication *tspm = new tspm_udp_communication (MY_IP);
  tspm->init();

  unsigned short reg;
  unsigned int val = 0;
  unsigned int val2 = 0;
  int i;

  if ( argc > 2 )
    {

      if ( strcmp( argv[1], "udpheader1") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  i = tspm->set_value( 9, val);
	}

      else if ( strcmp( argv[1], "udpheader2") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  i = tspm->set_value( 10, val);
	}

      else if ( strcmp( argv[1], "reset-spi") == 0)  
	{
	  reg = get_uvalue( argv[2]) & 0x7  ;
	  i = tspm->reset_spi( val);
	}

      else if ( strcmp( argv[1], "set-asic-disable-mask") == 0)  
	{
	  val = get_uvalue( argv[2]);
	  i = tspm->set_value( 1, val);
	}

      else if ( strcmp( argv[1], "set-all-dacs") == 0)  
	{
	  if ( argc < 4) exithelp();

	  val = get_uvalue( argv[2]) & 0xffff;
	  val2 = get_uvalue( argv[3]) & 0xffff;
	  unsigned int x = val;
	  x <<= 16;
	  x+= val2;
	  i = tspm->set_multiplevalues( 24, 11, x);
	}

      else
	{
	  reg = get_uvalue( argv[1]);
	  val = get_uvalue( argv[2]);

	  i = tspm->set_value( reg, val);
	  cout << " set of register " << reg << " to " << val <<  " 0x" << hex << val << dec << " status = " << i << endl;
	}
    }

  else
    {

      if ( strcmp( argv[1], "reset") == 0)  
	{
	  i = tspm->set_value( 0, 1);
	}

      else if ( strcmp( argv[1], "help") == 0)  
	{
	  exithelp(0);
	}

      else if ( strcmp( argv[1], "udpheader1") == 0)  
	{
	  i = tspm->get_value( 9, &val);
	  cout << " udpheader1  status = " << i << " value  " << val << "  0x" << hex << val << endl;
	}

      else if ( strcmp( argv[1], "udpheader2") == 0)  
	{
	  i = tspm->get_value( 10, &val);
	  cout << " udpheader2  status = " << i << " value  " << val << "  0x" << hex << val << endl;
	}

      else if ( strcmp( argv[1], "disable-all-asics") == 0)  
	{
	  i = tspm->set_value( 1, 0xffffff);
	}

      else
	{
	  reg = atoi (argv[1]);
	  i = tspm->get_value( reg, &val);
	  cout << " register " << reg << " status = " << i << " value  " << val << "  0x" << hex << val << endl;
	}

    }
  return 0;
}
