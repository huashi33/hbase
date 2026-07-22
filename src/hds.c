#include "hds.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// BUF
#define HBUF_DATA_SIZE_MAX 1024 * 1024 * 1024  // 1 GB
#define HBUF_DATA_SIZE_MIN 512                 // 512 byte

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



int hhash_init(hhash_t*,size_t){return HBASE_RET_OK;}
int hhash_deinit(hhash_t*){return HBASE_RET_OK;}
int hhash_set(hhash_t *h,const char* key,hlistitem_t* target){return HBASE_RET_OK;}
int hhash_get(hhash_t *h,const char* key,hlistitem_t* target){return HBASE_RET_OK;}