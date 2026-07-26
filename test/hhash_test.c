#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "hds.h"
// typedef struct myitem_ {
//   int val;
// } myitem_t;

// static void test_init_deinit(void) {
//   hhash_t h;
//   assert(hhash_init(&h) == HBASE_RET_OK);
//   assert(hhash_len(&h) == 0);
//   assert(hhash_deinit(&h) == HBASE_RET_OK);
//   printf("[PASS] test_init_deinit\n");
// }

// static void test_set_get(void) {
//   hhash_t h;
//   myitem_t a = {.val = 42};
//   myitem_t b = {.val = 99};

//   hhash_init(&h);

//   //  set + get
//   assert(hhash_set(&h, "alpha", &a) == HBASE_RET_OK);
//   assert(hhash_set(&h, "beta", &b) == HBASE_RET_OK);
//   assert(hhash_len(&h) == 2);

//   myitem_t* p = (myitem_t*)hhash_get(&h, "alpha");
//   assert(p != NULL);
//   assert(p->val == 42);

//   p = (myitem_t*)hhash_get(&h, "beta");
//   assert(p != NULL);
//   assert(p->val == 99);

//   // 不存在的 key 返回 NULL
//   assert(hhash_get(&h, "gamma") == NULL);

//   hhash_deinit(&h);
//   printf("[PASS] test_set_get\n");
// }

// static void test_overwrite(void) {
//   hhash_t h;
//   myitem_t a1 = {.val = 1};
//   myitem_t a2 = {.val = 2};

//   hhash_init(&h);
//   hhash_set(&h, "key", &a1);
//   hhash_set(&h, "key", &a2); /* 覆盖 */

//   assert(hhash_len(&h) == 1); /* 数量不变 */
//   myitem_t* p = (myitem_t*)hhash_get(&h, "key");
//   assert(p->val == 2); /* 值已更新 */

//   hhash_deinit(&h);
//   printf("[PASS] test_overwrite\n");
// }

// static void test_del(void) {
//   hhash_t h;
//   myitem_t a = {.val = 7};

//   hhash_init(&h);
//   hhash_set(&h, "x", &a);
//   assert(hhash_len(&h) == 1);

//   assert(hhash_del(&h, "x") == HBASE_RET_OK);
//   assert(hhash_len(&h) == 0);
//   assert(hhash_get(&h, "x") == NULL);

//   /* 删除不存在的 key 应返回非 OK 错误码 */
//   int r = hhash_del(&h, "x");
//   assert(r != HBASE_RET_OK);

//   hhash_deinit(&h);
//   printf("[PASS] test_del\n");
// }

// static void test_geti(void) {
//   hhash_t h;
//   myitem_t items[3] = {{.val = 10}, {.val = 20}, {.val = 30}};
//   const char* keys[3] = {"k0", "k1", "k2"};

//   hhash_init(&h);
//   for (int i = 0; i < 3; ++i) hhash_set(&h, keys[i], &items[i]);

//   assert(hhash_len(&h) == 3);

//   /* 遍历所有条目 */
//   for (ptrdiff_t i = 0; i < hhash_len(&h); ++i) {
//     const char* k = hhash_geti_key(&h, (size_t)i);
//     myitem_t* v = (myitem_t*)hhash_geti(&h, (size_t)i);
//     assert(k != NULL);
//     assert(v != NULL);
//     printf("  [%td] key=%s val=%d\n", i, k, v->val);
//   }

//   /* 越界访问返回 NULL */
//   assert(hhash_geti(&h, 99) == NULL);
//   assert(hhash_geti_key(&h, 99) == NULL);

//   hhash_deinit(&h);
//   printf("[PASS] test_geti\n");
// }

// static void test_null_params(void) {
//   hhash_t h;
//   hhash_init(&h);

//   /* NULL 参数不应崩溃 */
//   assert(hhash_set(&h, NULL, NULL) != HBASE_RET_OK);
//   assert(hhash_get(&h, NULL) == NULL);
//   assert(hhash_del(&h, NULL) != HBASE_RET_OK);
//   assert(hhash_len(NULL) == 0);
//   assert(hhash_geti(NULL, 0) == NULL);
//   assert(hhash_geti_key(NULL, 0) == NULL);

//   hhash_deinit(&h);
//   printf("[PASS] test_null_params\n");
// }

typedef struct hdval_ {
  uint8_t type;
  uint8_t ph0;
  uint16_t ph1;
  uint32_t ph2;
  void* v;
} hdval_t;

HHASH_DEFINE(char*, hdval_t)

// #define STB_DS_IMPLEMENTATION
static void hhash_test() {
  int r = 0;
  hhash_hdval_t* hash_hdval = hhash_init_hdval_t();
  printf("[%08X]hhash_init_hdval_t:%p\n", r, hash_hdval);

  hdval_t v;
  v.type = 21;
  r = hhash_set_hdval_t(hash_hdval, "name", v);
  printf("[%08X]hhash_set_hdval_t\n", r);

  clock_t start = clock();
  hdval_t* pv = NULL;
  for (int i = 0; i < 1000000; ++i) {
    pv = hhash_get_hdval_t(hash_hdval, "name");
  }

  clock_t end = clock();
  double sec = (double)(end - start) / CLOCKS_PER_SEC;
  printf("cost = %.6f s\n", sec);

  if (pv) {
    printf("[%08X]hhash_get_hdval_t,type:%d\n", r, pv->type);
  }

  r = hhash_deinit_hdval_t(hash_hdval);
  printf("[%08X]hhash_free_hdval_t\n", r);
}

int main(void) {
  // test_init_deinit();
  // test_set_get();
  // test_overwrite();
  // test_del();
  // test_geti();
  // test_null_params();
  printf("All hhash tests passed.\n");
  hhash_test();

  return 0;
}
