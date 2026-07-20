#include <stdio.h>
#include "hcfg.h"

int main(int argc,char *argv[]){
  const char* cfgname = argv[1];
  const char* key = argv[2];


  hcfg_t c;
  int r = 0;
  r = hcfg_init(&c,cfgname,HCFG_TYPE_INI);
  HBASE_RET_WHEN(r, r);

  char vv[256] = {0};
  r = hcfg_str(&c,key,vv,sizeof(vv));
  printf("%s,%s -> %s\n",cfgname,key,vv);

  r = hcfg_deinit(&c);
  HBASE_RET_WHEN(r, r);
  return 0;
}