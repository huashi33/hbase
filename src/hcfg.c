#include "hcfg.h"
#include <string.h>
#include "iniparser.h"

int hcfg_init(hcfg_t* c, const char* filename, uint8_t cfg_type) {
  HC_RET_WHEN(!c, HC_RET(HC_RET_BASE_CFG, HC_RET_PARAM(0)));
  HC_RET_WHEN(!filename, HC_RET(HC_RET_BASE_CFG, HC_RET_PARAM(1)));
  HC_RET_WHEN(HCFG_TYPE_END <= cfg_type,
                 HC_RET(HC_RET_BASE_CFG, HC_RET_PARAM(2)));

  if (HCFG_TYPE_INI == cfg_type) {
    dictionary* ini = iniparser_load(filename);
    HC_RET_WHEN(!ini, HC_RET(HC_RET_BASE_CFG, HC_RET_INIT));
    c->cfg_type = cfg_type;
    c->handle = ini;
    c->size = ini->n;
    return HC_RET_OK;
  }
  return HC_RET(HC_RET_BASE_CFG, HC_RET_NOTFOUND);
}
int hcfg_deinit(hcfg_t* c) {
  HC_RET_WHEN(!c, HC_RET(HC_RET_BASE_CFG, HC_RET_PARAM(0)));
  dictionary* ini = (dictionary*)c->handle;
  iniparser_freedict(ini);
  return HC_RET_OK;
}

int hcfg_str(hcfg_t* c, const char* key, char* val, size_t val_size) {
  HC_RET_WHEN(!c || !c->handle,
                 HC_RET(HC_RET_BASE_CFG, HC_RET_PARAM(0)));
  HC_RET_WHEN(!key, HC_RET(HC_RET_BASE_CFG, HC_RET_PARAM(1)));
  HC_RET_WHEN(!val, HC_RET(HC_RET_BASE_CFG, HC_RET_PARAM(2)));
  HC_RET_WHEN(!val_size, HC_RET(HC_RET_BASE_CFG, HC_RET_PARAM(3)));

  const char* s = iniparser_getstring(c->handle, key, NULL);
  HC_RET_WHEN(!s, HC_RET(HC_RET_BASE_CFG, HC_RET_NOTFOUND));

  strncpy(val, s, val_size-1);
  val[val_size-1] = 0;
  return HC_RET_OK;
}