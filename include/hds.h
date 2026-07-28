#ifndef _HDS_H_
#define _HDS_H_
#include "hdef.h"

// BUF
#define HC_RET_BUF_BASE HC_RET_BASE_DS
typedef struct hbuf_ {
  size_t len;
  size_t size;
  uint8_t* data;
} hbuf_t;
HFC_EXPORT int hbuf_init(hbuf_t* b, size_t s);
HFC_EXPORT int hbuf_deinit(hbuf_t* b);
HFC_EXPORT int hbuf_push(hbuf_t* b, const void* d, size_t dSize);
HFC_EXPORT int hbuf_clear(hbuf_t* b);
HFC_EXPORT int hbuf_print(hbuf_t* b, size_t ofs, size_t size);

// RQ
// TODO cache alive!!
#define HC_RET_BASE_RQ HC_RET_BASE_DS
typedef struct hrq_ {
  size_t size;  // 2^n
  size_t in;
  size_t out;
  uint8_t* data;
} hrq_t;
HFC_EXPORT int hrp_init(hrq_t* rq, uint32_t s);
HFC_EXPORT int hrp_deinit(hrq_t* rq);
HFC_EXPORT int hrp_push(hrq_t* rq, void* data, uint32_t size);
HFC_EXPORT int hrp_pop(hrq_t* rq, void* data, uint32_t size);

// LIST
#define HC_RET_BASE_LIST HC_RET_BASE_DS
typedef struct hlistitem_ {
  struct hlistitem_* next;
} hlistitem_t;
typedef struct hlist_ {
  hlistitem_t* head;
  size_t size;
} hlist_t;
typedef int (*hlist_compare_t)(hlistitem_t* i, hlistitem_t* target);
typedef int (*hlist_foreach_t)(hlistitem_t* i, int index);

// without mem free
HFC_EXPORT int hlist_clear(hlist_t*);
HFC_EXPORT int hlist_append(hlist_t* l, hlistitem_t* i);
HFC_EXPORT int hlist_insert(hlist_t* l, hlistitem_t* i, uint32_t index);
HFC_EXPORT hlistitem_t* hlist_at(hlist_t* l, uint32_t index);
HFC_EXPORT hlistitem_t* hlist_remove(hlist_t* l, uint32_t index);
// need set compare-func
HFC_EXPORT hlistitem_t* hlist_remove_first(hlist_t* l, hlistitem_t* tar,
                                           hlist_compare_t cmp);
HFC_EXPORT hlistitem_t* hlist_find_first(hlist_t* l, hlistitem_t* tar,
                                         hlist_compare_t cmp);
// need set foreach-func
HFC_EXPORT int hislist_foreach(hlist_t* l, hlist_foreach_t f);

// HASH
#define HC_RET_BASE_HASH HC_RET_BASE_DS

#include "stb_ds.h"

#define HHASH_TYPEDEF(TK, TV) hhash_##TV

#define HHASH_DEFINE_ENTRY(TK, TV) \
  typedef struct hhash_##TV##_ {   \
    TK key;                        \
    TV value;                      \
  } HHASH_TYPEDEF(TK, TV);

#define HHASH_DEFINE_INIT(TK, TV)                           \
  static inline HHASH_TYPEDEF(TK, TV) * hhash_init_##TV() { \
    HHASH_TYPEDEF(TK, TV)* h = NULL;                        \
    sh_new_strdup(h);                                       \
    return h;                                               \
  }

#define HHASH_DEFINE_DEINIT(TK, TV)                                         \
  static inline int hhash_deinit_##TV(HHASH_TYPEDEF(TK, TV) * h) {          \
    HC_RET_WHEN(!h, HC_RET(HC_RET_BASE_HASH, HC_RET_PARAM(0))); \
    shfree(h);                                                              \
    return HC_RET_OK;                                                    \
  }

#define HHASH_DEFINE_GET(TK, TV)                                      \
  static inline TV* hhash_get_##TV(HHASH_TYPEDEF(TK, TV) * h, TK k) { \
    HC_RET_WHEN(!h, NULL);                                         \
    ptrdiff_t idx = shgeti(h, k);                                     \
    if (idx < 0) return NULL;                                         \
    return &h[idx].value;                                             \
  }

#define HHASH_DEFINE_SET(TK, TV)                                            \
  static inline int hhash_set_##TV(HHASH_TYPEDEF(TK, TV) * h, TK k, TV v) { \
    HC_RET_WHEN(!h, HC_RET(HC_RET_BASE_HASH, HC_RET_PARAM(0))); \
    shput(h, k, v);                                                         \
    return HC_RET_OK;                                                    \
  }

#define HHASH_DEFINE_DEL(TK, TV)                                            \
  static inline int hhash_del_##TV(HHASH_TYPEDEF(TK, TV) * h, TK k) {       \
    HC_RET_WHEN(!h, HC_RET(HC_RET_BASE_HASH, HC_RET_PARAM(0))); \
    int found = shdel(h, k);                                                \
    return found ? HC_RET_OK                                             \
                 : HC_RET(HC_RET_BASE_HASH, HC_RET_NOTFOUND);      \
  }

#define HHASH_DEFINE_SIZE(TK, TV)                                   \
  static inline size_t hhash_size_##TV(HHASH_TYPEDEF(TK, TV) * h) { \
    return h ? (size_t)shlen(h) : 0;                                \
  }

#define HHASH_DEFINE_EXIST(TK, TV)                                        \
  static inline int hhash_exist_##TV(HHASH_TYPEDEF(TK, TV) * h, TK k) {   \
    HC_RET_WHEN(!h, HC_RET(HC_RET_BASE_HASH, HC_RET_PARAM(0))); \
    ptrdiff_t idx = shgeti(h, k);                                         \
    return idx >= 0 ? HC_RET_OK                                        \
                    : HC_RET(HC_RET_BASE_HASH, HC_RET_NOTFOUND); \
  }

#define HHASH_DEFINE(TK, TV)  \
  HHASH_DEFINE_ENTRY(TK, TV)  \
  HHASH_DEFINE_INIT(TK, TV)   \
  HHASH_DEFINE_DEINIT(TK, TV) \
  HHASH_DEFINE_GET(TK, TV)    \
  HHASH_DEFINE_SET(TK, TV)    \
  HHASH_DEFINE_DEL(TK, TV)    \
  HHASH_DEFINE_SIZE(TK, TV)   \
  HHASH_DEFINE_EXIST(TK, TV)

#endif
