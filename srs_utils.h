#ifndef __SRS_UTILS_H__
#define __SRS_UTILS_H__

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



int read_params ( const char *ip, const int regport, const int regcount,  returnstructure *returnblock, const int subaddress =0);
int read_parameter ( const char *ip, const int regport, const unsigned int adr,  returnstructure *returnblock, const int subaddress=0);
int set_value( const char *ip, const unsigned int adr, const unsigned int subaddress, const int reqport, const unsigned int val);

#endif
