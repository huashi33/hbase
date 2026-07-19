#include "hlog.h"

#include "zlog.h"
// static zlog_category_t* cat_in_lib;

int hlog_init(const char *appname,const char* cfgfilename) {
  HBASE_RET_WHEN(!appname, HBASE_RET(HBASE_RET_BASE_LOG, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(!cfgfilename, HBASE_RET(HBASE_RET_BASE_LOG, HBASE_RET_PARAM(1)));

  int r = 0;
  r = dzlog_init(cfgfilename, appname);
  HBASE_RET_WHEN(r, HBASE_RET(HBASE_RET_BASE_LOG, HBASE_RET_INIT));

  dzlog_info("init %s", appname);
  return HBASE_RET_OK;
}

int hlog_deinit(const char *appname) { 

  dzlog_info("deinit %s", appname);
  zlog_fini();
  return HBASE_RET_OK;
}

