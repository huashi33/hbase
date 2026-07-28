#include "hlog.h"

#include "zlog.h"
// static zlog_category_t* cat_in_lib;

int hlog_init(const char *appname,const char* cfgfilename) {
  HC_RET_WHEN(!appname, HC_RET(HC_RET_BASE_LOG, HC_RET_PARAM(0)));
  HC_RET_WHEN(!cfgfilename, HC_RET(HC_RET_BASE_LOG, HC_RET_PARAM(1)));

  int r = 0;
  r = dzlog_init(cfgfilename, appname);
  HC_RET_WHEN(r, HC_RET(HC_RET_BASE_LOG, HC_RET_INIT));

  dzlog_info("init %s", appname);
  return HC_RET_OK;
}

int hlog_deinit(const char *appname) { 

  dzlog_info("deinit %s", appname);
  // the lib will register atexit and call zlog_fini
  // zlog_fini();
  return HC_RET_OK;
}

