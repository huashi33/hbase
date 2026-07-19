#ifndef _HCFG_H_
#define _HCFG_H_
#include "hdef.h"




#define HCFG_TYPE_CONF  0
#define HCFG_TYPE_INI   1
typedef struct hcfg_{
  uint8_t   cfg_type;
  uint32_t  size;
  void*     data;
}hcfg_t;
int hcfg_init(hcfg_t* c,const char *filename,uint8_t cfg_type);
int hcfg_deinit(hcfg_t* c);
int hcfg_getstr(hcfg_t* c,const char* key);
int hcfg_getint(hcfg_t* c,const char* key);
int hcfg_getdouble(hcfg_t* c,const char* key);



// htime 
typedef struct htime_{
  uint16_t  year;
  uint8_t   mon;
  uint8_t   day;
  uint8_t   hour;
  uint8_t   min;
  uint8_t   sec;
  uint16_t  millsec;
}htime_t;

// ms since 1970
int htime_utc(uint64_t *t);
int htime_utc2htime(const uint64_t t,htime_t* ht);
int htime_htime2utc(uint64_t *t,const htime_t* ht);


#endif