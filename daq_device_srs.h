#ifndef __DAQ_DEVICE_SRS_H__
#define __DAQ_DEVICE_SRS_H__


#include <daq_device.h>
#include <srsTriggerHandler.h>
#include <iostream>

#include <arpa/inet.h>
//#include <netinet/in.h>

class daq_device_srs : public  daq_device {

public:

  daq_device_srs (const int eventtype
		  , const int subeventid
		  , const char *ipaddr
		  , const int trigger =1);

  ~daq_device_srs();


  void identify(std::ostream& os = std::cout) const;

  int max_length(const int etype) const;

  // functions to do the work

  int put_data(const int etype, int * adr, const int length);

  int init();
  int endrun();

  int rearm( const int etype);

protected:
  subevtdata_ptr sevt;
  unsigned int number_of_words;

  char _ip[512];
  int _s;
  struct sockaddr_in _si_me;
  int _broken;

  srsTriggerHandler *_th;
  fd_set read_flag;

};

#endif

