#ifndef __SRSTRIGGERHANDLER_H__
#define __SRSTRIGGERHANDLER_H__

#include <sys/select.h>
#include "TriggerHandler.h"


class srsTriggerHandler : public TriggerHandler {

public:

  srsTriggerHandler(fd_set *theset, const int highest_fd)
    {
      _highest_fd = highest_fd;
      _fdset = theset;

    }

  ~srsTriggerHandler() {};

  //  virtual void identify(std::ostream& os = std::cout) const = 0;

  // this is the virtual worker routine
  int wait_for_trigger( const int moreinfo=0);

 protected:
  
  int _highest_fd;
  fd_set *_fdset;

};

#endif
