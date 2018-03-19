#include "daq_device_srs.h"
#include "srs_utils.h"

#include <unistd.h>
#include <string.h>

#define PORT 6006

// max 16 hybrids + 1 end marker + 1 reserve
#define MAX_PACKETS 18

using namespace std;

daq_device_srs::daq_device_srs(const int eventtype
			       , const int subeventid
			       , const char *ipaddr
			       , const int trigger)
{

  m_eventType  = eventtype;
  m_subeventid = subeventid;
  strcpy ( _ip, ipaddr);
  _s = 0;
  _th = 0;


  _broken = 0;


  if ((_s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    {
      _broken =1;
      cout << __LINE__ << "  " << __FILE__ << " broken 1" << endl;
      perror("srs");
    }

  memset((char *) &_si_me, 0, sizeof(_si_me));
  _si_me.sin_family = AF_INET;
  _si_me.sin_port = htons(PORT);
  _si_me.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(_s, (sockaddr *)&_si_me, sizeof(_si_me))==-1)
   {
      _broken =1;
      cout << __LINE__ << "  " << __FILE__ << " broken 1" << endl;
      perror("bind");

    }

  FD_ZERO(&read_flag);
  FD_SET(_s, &read_flag);


  if ( trigger) 
    {
      //  cout << __LINE__ << "  " << __FILE__ << " registering triggerhandler " << endl;
      _th  = new srsTriggerHandler ( &read_flag, _s);
      registerTriggerHandler ( _th);
    }

}

daq_device_srs::~daq_device_srs()
{
  if ( _th) 
    {
      clearTriggerHandler();
      delete _th;
      _th = 0;
    }
  close( _s );
}



// the put_data function

int daq_device_srs::put_data(const int etype, int * adr, const int length )
{
  //  cout << __LINE__ << "  " << __FILE__ << " etype= " << etype << " adr= " << adr << " length= " << length << endl;

  if ( _broken ) 
    {
      cout << __LINE__ << "  " << __FILE__ << " broken ";
      identify();
      return 0; //  we had a catastrophic failure
    }

  int len = 0;

  if (etype != m_eventType )  // not our id
    {
      return 0;
    }

  sevt =  (subevtdata_ptr) adr;
  // set the initial subevent length
  sevt->sub_length =  SEVTHEADERLENGTH;

  // update id's etc
  sevt->sub_id =  m_subeventid;
  sevt->sub_type=4;
  sevt->sub_decoding = 70;
  sevt->reserved[0] = 0;
  sevt->reserved[1] = 0;

  if ( length < max_length(etype) ) 
    {
      cout << __LINE__ << "  " << __FILE__ << " length " << length <<endl;
      return 0;
    }

  unsigned int data;

  char *d = (char *) &sevt->data;

  struct timeval tv;

  struct sockaddr_in si_src;
  socklen_t src_len;
  int ia;
  int received_len; 

  for ( ia = 0; ia < MAX_PACKETS; ia++)  //receive 
    {

      tv.tv_sec = 1;
      tv.tv_usec = 0;
      int retval = select(_s+1, &read_flag, NULL, NULL, &tv);
      if ( retval > 0) 
      	{
	  src_len = sizeof (si_src);
	  if ( ( received_len = recvfrom(_s, d, 9000, 0, (sockaddr *) &si_src, &src_len) ) ==-1)
	    {
	      perror ( "receive ");
	      cout << __LINE__ << "  " << __FILE__ << " error " << received_len <<endl;
	      return 0;
	    }
	  //  cout << __LINE__ << "  " << __FILE__ << " got " << received_len << " bytes " << _s << endl;

	  len += received_len;   // we are still counting bytes here
	  d+= received_len;     // and here - update the write pointer

	  if (received_len == 4) 
	    {
	      len /= 4;  // now we have ints
	      
	      sevt->sub_padding = len%2;
	      len = len + (len%2);
	      sevt->sub_length += len;
	      //  cout << __LINE__ << "  " << __FILE__ << " returning "  << sevt->sub_length << endl;
	      return  sevt->sub_length;
	    }

	  const unsigned int eop_marker = 0xf000f000;
	  memcpy ( d, &eop_marker, 4);   // add the EOP
	  d+=4;
	  len+=4;

	}
      else
	{

	  cout << __LINE__ << "  " << __FILE__ << " " << retval << " timeout in ";
	  identify();
	  return 0;
	}

    }
  return 0;
}


void daq_device_srs::identify(std::ostream& os) const
{
  
  os  << "SRS Device  Event Type: " << m_eventType << " Subevent id: " << m_subeventid 
      << " IP: " << _ip;
  if ( _th) os << " trigger ";
  if ( _broken) os << " ** not functional ** ";
  os << endl;

}

int daq_device_srs::max_length(const int etype) const
{
  if (etype != m_eventType) return 0;
  return  (MAX_PACKETS*2255 + SEVTHEADERLENGTH);
}

int  daq_device_srs::init()
{
  //execute the "RO_ENABLE" function, reg 15 on port 6039 ->1
  int i = set_value( _ip, 0xF, 0, 6039, 1);
 
  return 0;

}
int  daq_device_srs::endrun()
{
  //execute the "RO_DISABLE" function, reg 15 on port 6039 ->0
  int i = set_value( _ip, 0xF, 0, 6039, 0);
  return 0;
}

// the rearm() function
int  daq_device_srs::rearm(const int etype)
{
  if (etype != m_eventType) return 0;
  return 0;
}


