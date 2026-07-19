#ifndef _HDS_H_
#define _HDS_H_
#include "hdef.h"

// BUF
#define HBASE_RET_BUF_BASE  HBASE_RET_BASE_DS
typedef struct hbuf_{
    size_t      len;
    size_t      size;
    uint8_t*    data;
}hbuf_t;
HFC_EXPORT int hbuf_init(hbuf_t *b,size_t s);
HFC_EXPORT int hbuf_deinit(hbuf_t *b);
HFC_EXPORT int hbuf_push(hbuf_t *b,const void *d,size_t dSize);
HFC_EXPORT int hbuf_print(hbuf_t *b,size_t ofs,size_t size);


// RQ
// TODO cache alive!!
#define HBASE_RET_BASE_RQ  HBASE_RET_BASE_DS
typedef struct hrq_{
    size_t      size;//2^n
    size_t      in;
    size_t      out;
    uint8_t*    data;
}hrq_t;
HFC_EXPORT int hrp_init(hrq_t *rq,uint32_t s);
HFC_EXPORT int hrp_deinit(hrq_t* rq);
HFC_EXPORT int hrp_push(hrq_t* rq, void* data, uint32_t size);
HFC_EXPORT int hrp_pop(hrq_t* rq, void* data, uint32_t size);


// LIST
#define HBASE_RET_BASE_LIST  HBASE_RET_BASE_DS
typedef struct hlistitem_{
    struct hlistitem_ *next;
}hlistitem_t;
typedef struct hlist_{
    hlistitem_t*    head;
    size_t          size;
}hlist_t;
typedef int(*hlist_compare_t)(hlistitem_t *i,hlistitem_t *target);
typedef int(*hlist_foreach_t)(hlistitem_t *i,int index);

// without mem free
HFC_EXPORT int hlist_clear(hlist_t *);
HFC_EXPORT int hlist_append(hlist_t *l,hlistitem_t *i);
HFC_EXPORT int hlist_insert(hlist_t *l,hlistitem_t *i,uint32_t index);
HFC_EXPORT hlistitem_t* hlist_at(hlist_t *l,uint32_t index);
HFC_EXPORT hlistitem_t* hlist_remove(hlist_t *l,uint32_t index);
//need set compare-func
HFC_EXPORT hlistitem_t* hlist_remove_first(hlist_t *l,hlistitem_t *tar,hlist_compare_t cmp);
HFC_EXPORT hlistitem_t* hlist_find_first(hlist_t *l,hlistitem_t *tar,hlist_compare_t cmp);
//need set foreach-func
HFC_EXPORT int hislist_foreach(hlist_t *l,hlist_foreach_t f);


// HASH
#define HBASE_RET_BASE_HASH  HBASE_RET_BASE_DS

typedef struct hhash_{
    size_t key_size;

}hhash_t;


int hhash_init(hhash_t*,size_t);
int hhash_deinit(hhash_t*);
int hhash_set(hhash_t *h,const char* key,hlistitem_t* target);
int hhash_get(hhash_t *h,const char* key,hlistitem_t* target);


#endif
