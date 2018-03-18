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


using namespace std;


main(int argc, char *argv[])
{

  if ( argc < 2)
    {
      cout << "Usage: " << argv[0] << " file" << endl;
      exit(1);
    }


  int fd =  open(argv[1], O_RDONLY | O_LARGEFILE );
  if (fd < 0)
    {
      cerr << "could not open " << argv[1] << endl;
      exit(1);
    }

  int nevt = 2;
  if  ( argc == 3)
    {
      nevt = atoi (argv[2]);
    }


  int n;
  unsigned int d;
  for ( int event = 0; event <nevt; event++)
    {

      int go_on =1;
      while (go_on)
	{
	  n = read( fd, &d, 4);
	  if ( d == 0xfafafafa ) break;
	  unsigned int dh = ntohl(d);
	  //	  cout << "framecounter: " << (dh & 0xff) << endl;
      
	  n = read( fd, &d, 4);
	  dh = ntohl(d);
	  //cout << "Header: " << hex << dh << dec << "  " ;
      
	  unsigned char c = (dh >>24) & 0xff;
	  //cout << c;
	  c = (dh >>16) & 0xff;
	  //cout << c;
	  c = (dh >>8) & 0xff;
	  //cout << c;
      
	  unsigned int channel = dh & 0xff;
	  //cout << " channel nr: " << channel << endl;
      
	  n = read( fd, &d, 4);
	  unsigned int header = ntohl(d);
	  unsigned int words = header & 0xffff;
	  //cout << " words: " << words << endl;
	  
	  unsigned short val;

	  int i = 0;

	  n = read ( fd, &d, 4);
	  while ( d != 0xf000f000 && i < words +1)
	    {
	      val = ( d & 0xffff);
	      cout << setw(6) << i++ << setw(6) << val;
	      if ( val < 1200 ) cout << " D 1";
	      else if ( val > 3000 ) cout << " D0";
	      else  cout << "  ";
	      cout << endl;
	      
	      val = ( (d >>16 ) & 0xffff);
	      cout << setw(6) << i++ << setw(6) << val;
	      if ( val < 1200 ) cout << " D 1";
	      else if ( val > 3000 ) cout << " D0";
	      else  cout << "  ";
	      cout << endl;
	      
	      n = read( fd, &d, 4);
	    }

	}
      n = read( fd, &d, 4);
      if ( d != 0xf000f000 ) cout << " no EOP marker found " << hex << d << dec << endl; 
      //cout << " end of event " << event << endl;
    }

  close (fd);
  return 0;

}
