#include "srs_plugin.h"
#include <daq_device_srs.h>

#include <strings.h>

int srs_plugin::create_device(deviceblock *db)
{

  //std::cout << __LINE__ << "  " << __FILE__ << "  " << db->argv0 << std::endl;

  if ( strcasecmp(db->argv0,"device_srs") == 0 ) 
    {
      // we need at least 3 params
      if ( db->npar <4 ) return 1; // indicate wrong params
      
      int eventtype  = atoi ( db->argv1); // event type
      int subid = atoi ( db->argv2); // subevent id


      if ( db->npar ==4 ) 
	{
	  daq_device *x = new daq_device_srs( eventtype,
					      subid,
					      db->argv3 );
	  //      x->identify();
	  
	  add_readoutdevice (x);
	  return 0;  // say "we handled this request" 
	}  

      else if ( db->npar ==5 ) 
	{
	  daq_device *x = new daq_device_srs( eventtype,
					      subid,
					      db->argv3,
					      atoi (db->argv4) );
	  //      x->identify();
	  
	  add_readoutdevice (x);
	  return 0;  // say "we handled this request" 
	}  


    }
  
  return -1; // say " this is not our device"
}

void srs_plugin::identify(std::ostream& os, const int flag) const
{

  if ( flag <=2 )
    {
      os << " - SRS Plugin" << std::endl;
    }
  else
    {
      os << " - SRS Plugin, provides - " << std::endl;
      os << " -     device_srs (evttype, subid, IP addr) - readout an SRS crate " << std::endl;

    }
      
}

srs_plugin _sp;

