#include <stdio.h>
#include "hlog.h"

int main(int argc,char *argv[]){
  const char* appname = argv[1];
  const char* cfgname = argv[2];

  int r = 0;
  r = hlog_init(appname,cfgname);
  HBASE_RET_WHEN(r, r);

  HLOG_DEBUG("log from d");
  HLOG_INFO("log from i");
  HLOG_WARN("log from w");
  HLOG_ERROR("log from e");

  r = hlog_deinit(appname);
  HBASE_RET_WHEN(r, r);
  return 0;
}