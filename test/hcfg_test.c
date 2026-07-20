#include <stdio.h>
#include "hcfg.h"
#include "dictionary.h"

static int dic_test(){
  int r = 0;
  dictionary *dic = dictionary_new(0);

  const char *k="name";
  char vv[256] = "";

  r = dictionary_set(dic,k,"zhangsan");
  HBASE_RET_WHEN(r,r);

  const char *vget = dictionary_get(dic,k,NULL);
  HBASE_RET_WHEN(!vget,1);
  printf("%s:%s\n",k,vget);


  return 0;
}
static int cfg_test(const char* cfgname,const char* key ){
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
int main(int argc,char *argv[]){
  int r =0;
  r = cfg_test(argv[1],argv[2]);

  r = dic_test();
  return r;
}