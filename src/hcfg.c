#include "hcfg.h"

#include <string.h>

#include "iniparser.h"

int hcfg_init(hcfg_t* c, const char* filename, uint8_t cfg_type) {
  HBASE_RET_WHEN(!c, HBASE_RET(HBASE_RET_BASE_CFG, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(!filename, HBASE_RET(HBASE_RET_BASE_CFG, HBASE_RET_PARAM(1)));
  HBASE_RET_WHEN(HCFG_TYPE_END <= cfg_type,
                 HBASE_RET(HBASE_RET_BASE_CFG, HBASE_RET_PARAM(2)));

  if (HCFG_TYPE_INI == cfg_type) {
    dictionary* ini = iniparser_load(filename);
    HBASE_RET_WHEN(!ini, HBASE_RET(HBASE_RET_BASE_CFG, HBASE_RET_INIT));
    c->cfg_type = cfg_type;
    c->handle = ini;
    c->size = ini->n;
    return HBASE_RET_OK;
  }
  return HBASE_RET(HBASE_RET_BASE_CFG, HBASE_RET_NOTFOUND);
}
int hcfg_deinit(hcfg_t* c) {
  HBASE_RET_WHEN(!c, HBASE_RET(HBASE_RET_BASE_CFG, HBASE_RET_PARAM(0)));
  dictionary* ini = (dictionary*)c->handle;
  iniparser_freedict(ini);
  return HBASE_RET_OK;
}

int hcfg_str(hcfg_t* c, const char* key, char* val, size_t val_size) {
  HBASE_RET_WHEN(!c || !c->handle,
                 HBASE_RET(HBASE_RET_BASE_CFG, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(!key, HBASE_RET(HBASE_RET_BASE_CFG, HBASE_RET_PARAM(1)));
  HBASE_RET_WHEN(!val, HBASE_RET(HBASE_RET_BASE_CFG, HBASE_RET_PARAM(2)));
  HBASE_RET_WHEN(!val_size, HBASE_RET(HBASE_RET_BASE_CFG, HBASE_RET_PARAM(3)));

  const char* s = iniparser_getstring(c->handle, key, NULL);
  HBASE_RET_WHEN(!s, HBASE_RET(HBASE_RET_BASE_CFG, HBASE_RET_NOTFOUND));

  strncpy(val, s, val_size-1);
  val[val_size-1] = 0;
  return HBASE_RET_OK;
}