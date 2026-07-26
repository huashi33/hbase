

#include "hds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// BUF
#define HBUF_DATA_SIZE_MAX 1024 * 1024 * 1024  // 1 GB
#define HBUF_DATA_SIZE_MIN 32                 // 32 byte

int hbuf_init(hbuf_t* b, size_t s) {
  HBASE_RET_WHEN(!b, HBASE_RET(HBASE_RET_BUF_BASE, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(HBUF_DATA_SIZE_MAX < s,
                 HBASE_RET(HBASE_RET_BUF_BASE, HBASE_RET_PARAM(2)));

  b->size = HBUF_DATA_SIZE_MIN;
  HBASE_GETMINPOW(s, b->size);
  b->len = 0;
  b->data = (uint8_t*)HBASE_ALLOC(b->size);
  return HBASE_RET_OK;
}
int hbuf_deinit(hbuf_t* b) {
  HBASE_RET_WHEN(!b, HBASE_RET(HBASE_RET_BUF_BASE, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(!b->size, HBASE_RET(HBASE_RET_BUF_BASE, HBASE_RET_PARAM(1)));
  HBASE_RET_WHEN(!b->data, HBASE_RET(HBASE_RET_BUF_BASE, HBASE_RET_PARAM(2)));

  HBASE_FREE(b->data);
  b->len = b->size = 0;
  b->data = NULL;
  return HBASE_RET_OK;
}
static int hbuf_realloc(hbuf_t* b, size_t wish_size) {
  HBASE_RET_WHEN(wish_size <= b->size, HBASE_RET_OK);

  hbuf_t target = *b;
  b->size = HBUF_DATA_SIZE_MIN;
  HBASE_GETMINPOW(wish_size, target.size);
  HBASE_RET_WHEN(HBUF_DATA_SIZE_MAX < target.size,
                 HBASE_RET(HBASE_RET_BUF_BASE, HBASE_RET_NOSPACE));

  target.data = HBASE_REALLOC(target.data, target.size);
  HBASE_RET_WHEN(!target.data, HBASE_RET(HBASE_RET_BUF_BASE, HBASE_RET_MALLOC));

  *b = target;
  return HBASE_RET_OK;
}
int hbuf_push(hbuf_t* b, const void* d, size_t dsize) {
  HBASE_RET_WHEN(!b, HBASE_RET(HBASE_RET_BUF_BASE, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(!d, HBASE_RET(HBASE_RET_BUF_BASE, HBASE_RET_PARAM(1)));
  HBASE_RET_WHEN(!dsize, HBASE_RET(HBASE_RET_BUF_BASE, HBASE_RET_PARAM(2)));
  HBASE_RET_WHEN(!b->size, HBASE_RET(HBASE_RET_BUF_BASE, HBASE_RET_PARAM(3)));

  int r = HBASE_RET_OK;
  r = hbuf_realloc(b, b->len + dsize);
  HBASE_RET_WHEN(r, r);
  memcpy(b->data + b->len, d, dsize);
  b->len += dsize;
  return HBASE_RET_OK;
}

int hbuf_clear(hbuf_t *b){
  HBASE_RET_WHEN(!b, HBASE_RET(HBASE_RET_BUF_BASE, HBASE_RET_PARAM(0)));
  b->len = 0;
  return HBASE_RET_OK;
}

int hbuf_print(hbuf_t* b, size_t ofs, size_t size) {
  for (uint64_t i = ofs; size--; i++) {
    printf("%02X\t", b->data[i]);
  }
  return 0;
}

// RQ
#define HRQ_MIN(A, B) ((A) < (B) ? (A) : (B))
int hrp_init(hrq_t* rq, uint32_t s) {
  HBASE_RET_WHEN(!rq, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(!s, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(1)));
  HBASE_RET_WHEN(HBUF_DATA_SIZE_MAX < s,
                 HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(2)));

  rq->size = HBUF_DATA_SIZE_MIN;
  HBASE_GETMINPOW(s, rq->size);
  rq->data = (uint8_t*)HBASE_ALLOC(rq->size);
  rq->in = 0;
  rq->out = 0;
  return HBASE_RET_OK;
}
int hrp_deinit(hrq_t* rq) {
  HBASE_RET_WHEN(!rq, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(!rq->size, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(1)));
  HBASE_RET_WHEN(!rq->data, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(2)));
  HBASE_FREE(rq->data);
  rq->data = NULL;
  rq->in = rq->out = rq->size = 0;
  return HBASE_RET_OK;
}
int hrp_push(hrq_t* rq, void* data, uint32_t size) {
  HBASE_RET_WHEN(!rq, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(!data, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(1)));
  HBASE_RET_WHEN(!size, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(2)));
  HBASE_RET_WHEN(!rq->size, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(3)));

  uint32_t len = HRQ_MIN(rq->size - (rq->in - rq->out), size);
  uint32_t trueIn = rq->in & (rq->size - 1);
  uint32_t l1 = HRQ_MIN(len, rq->size - trueIn);
  memcpy(rq->data + trueIn, data, l1);
  memcpy(rq->data, (char*)data + l1, len - l1);
  rq->in += len;
  return len;
}
int hrp_pop(hrq_t* rq, void* data, uint32_t size) {
  HBASE_RET_WHEN(!rq, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(!data, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(1)));
  HBASE_RET_WHEN(!size, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(2)));
  HBASE_RET_WHEN(!rq->size, HBASE_RET(HBASE_RET_BASE_RQ, HBASE_RET_PARAM(3)));

  uint32_t len = HRQ_MIN(rq->in - rq->out, size);
  uint32_t trueOut = rq->out & (rq->size - 1);
  uint32_t l1 = HRQ_MIN(len, rq->size - trueOut);
  memcpy(data, rq->data + trueOut, l1);
  memcpy((char*)data + l1, rq->data, len - l1);
  rq->out += len;
  return len;
}

// LIST
int hlist_clear(hlist_t* l) {
  HBASE_RET_WHEN(!l, HBASE_RET(HBASE_RET_BASE_LIST, HBASE_RET_PARAM(0)));
  l->head = NULL;
  l->size = 0;
  return HBASE_RET_OK;
}

int hlist_append(hlist_t* l, hlistitem_t* i) {
  HBASE_RET_WHEN(!l, HBASE_RET(HBASE_RET_BASE_LIST, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(!i, HBASE_RET(HBASE_RET_BASE_LIST, HBASE_RET_PARAM(1)));

  i->next = NULL;
  hlistitem_t* cur = l->head;
  if (!cur) {
    l->head = i;
  } else {
    while (cur->next) {
      cur = cur->next;
    }
    cur->next = i;
  }
  ++l->size;
  return HBASE_RET_OK;
}
int hlist_insert(hlist_t* l, hlistitem_t* i, uint32_t index) {
  HBASE_RET_WHEN(!l, HBASE_RET(HBASE_RET_BASE_LIST, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(!i, HBASE_RET(HBASE_RET_BASE_LIST, HBASE_RET_PARAM(1)));
  HBASE_RET_WHEN(index >= l->size, HBASE_RET(HBASE_RET_BASE_LIST, HBASE_RET_PARAM(2)));


  if (0 == index) {
    i->next = l->head;
    l->head = i;
  } else {
    hlistitem_t* cur = l->head;
    for (size_t i = 0; i < index - 1; ++i) {
      cur = cur->next;
    }

    i->next = cur->next;
    cur->next = i;
  }
  ++l->size;
  return HBASE_RET_OK;
}
hlistitem_t* hlist_at(hlist_t* l, uint32_t index) {
  HBASE_RET_WHEN(!l, NULL);
  HBASE_RET_WHEN(index >= l->size, NULL);
  HBASE_RET_WHEN(0 == index, l->head);

  hlistitem_t* cur = l->head;
  for (size_t i = 0; i < index; ++i) {
    cur = cur->next;
  }
  return cur;
}
hlistitem_t* hlist_remove(hlist_t* l, uint32_t index) {
  HBASE_RET_WHEN(!l || index >= l->size, NULL);

  hlistitem_t* toRemove;
  if (0 == index) {
    toRemove = l->head;
    l->head = l->head->next;
  } else {
    hlistitem_t* cur = l->head;
    for (size_t i = 0; i < index - 1; ++i) {
      cur = cur->next;
    }
    toRemove = cur->next;
    cur->next = toRemove->next;
  }
  --l->size;
  return toRemove;
}
hlistitem_t* hlist_find_first(hlist_t* l, hlistitem_t* tar,hlist_compare_t cmp) {
  HBASE_RET_WHEN(!l || !tar || !cmp || !l->size, NULL);

  hlistitem_t* toRet = NULL;
  hlistitem_t* cur = l->head;
  for (size_t i = 0; i < l->size; ++i) {
    if (cmp(cur, tar)) {
      toRet = cur;
      break;
    }
    cur = cur->next;
  }
  return toRet;
}
hlistitem_t* hlist_remove_first(hlist_t* l, hlistitem_t* tar,hlist_compare_t cmp) {
  HBASE_RET_WHEN(!l || !tar || !cmp || !l->size, NULL);

  hlistitem_t* toRemove;
  if (cmp(l->head, tar)) {
    toRemove = l->head;
    l->head = l->head->next;
  } else {
    toRemove = l->head;
    hlistitem_t* cur = toRemove->next;
    for (size_t i = 0; i < l->size - 1; ++i) {
      if (cmp(cur, tar)) {
        toRemove->next = cur->next;
        toRemove = cur;
        break;
      }
      toRemove = cur;
      cur = cur->next;
    }
  }
  --l->size;
  return toRemove;
}
int hislist_foreach(hlist_t* l, hlist_foreach_t f) {
  HBASE_RET_WHEN(!l, HBASE_RET(HBASE_RET_BASE_LIST, HBASE_RET_PARAM(0)));
  HBASE_RET_WHEN(!f, HBASE_RET(HBASE_RET_BASE_LIST, HBASE_RET_PARAM(1)));

  hlistitem_t* it = l->head;
  for (int i = 0; i < l->size; ++i) {
    f(it, i);
    it = it->next;
  }
  return HBASE_RET_OK;
}



// HASH
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

// 基于 stb_ds.h 字符串哈希表（sh_new_strdup 模式）的封装实现。
// handle 类型为 hhash_entry_t*（与 stb_ds 的 struct{char*key; T value;}* 兼容）。

// int hhash_init(hhash_t *h) {
//     HBASE_RET_WHEN(!h, HBASE_RET(HBASE_RET_BASE_HASH, HBASE_RET_PARAM(0)));
//     h->handle = NULL;
//     /* sh_new_strdup：让 stb_ds 自动 strdup/free key，无需调用方管理 key 生命期 */
//     sh_new_strdup(h->handle);
//     return HBASE_RET_OK;
// }

// int hhash_deinit(hhash_t *h) {
//     HBASE_RET_WHEN(!h, HBASE_RET(HBASE_RET_BASE_HASH, HBASE_RET_PARAM(0)));
//     shfree(h->handle);   /* 释放哈希表及所有 strdup 拷贝的 key，handle 置 NULL */
//     h->handle = NULL;
//     return HBASE_RET_OK;
// }

// int hhash_set(hhash_t *h, const char *key, void *val) {
//     HBASE_RET_WHEN(!h,   HBASE_RET(HBASE_RET_BASE_HASH, HBASE_RET_PARAM(0)));
//     HBASE_RET_WHEN(!key, HBASE_RET(HBASE_RET_BASE_HASH, HBASE_RET_PARAM(1)));
//     /* stb_ds 宏要求 key 为 char*，强转掉 const（内部会 strdup，不会修改原字符串） */

//     shput(h->handle, (char *)key, val);
//     return HBASE_RET_OK;
// }

// void *hhash_get(hhash_t *h, const char *key) {
//     if (!h || !key) return NULL;
//     /* shgeti 返回 -1 表示不存在，避免与 value==NULL 混淆 */
//     ptrdiff_t idx = shgeti(h->handle, (char *)key);
//     if (idx < 0) return NULL;
//     return h->handle[idx].value;
// }

// int hhash_del(hhash_t *h, const char *key) {
//     HBASE_RET_WHEN(!h,   HBASE_RET(HBASE_RET_BASE_HASH, HBASE_RET_PARAM(0)));
//     HBASE_RET_WHEN(!key, HBASE_RET(HBASE_RET_BASE_HASH, HBASE_RET_PARAM(1)));
//     /* shdel 返回 1 表示删除成功，0 表示 key 不存在 */
//     int found = shdel(h->handle, (char *)key);
//     if (!found) return HBASE_RET(HBASE_RET_BASE_HASH, HBASE_RET_NOTFOUND);
//     return HBASE_RET_OK;
// }

// size_t hhash_len(hhash_t *h) {
//     if (!h) return 0;
//     return (size_t)shlen(h->handle);
// }

// void *hhash_geti(hhash_t *h, size_t index) {
//     if (!h) return NULL;
//     if ((ptrdiff_t)index >= shlen(h->handle)) return NULL;
//     return h->handle[index].value;
// }

// const char *hhash_geti_key(hhash_t *h, size_t index) {
//     if (!h) return NULL;
//     if ((ptrdiff_t)index >= shlen(h->handle)) return NULL;
//     return h->handle[index].key;
// }
