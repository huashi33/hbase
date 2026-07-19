#ifndef _HDEF_H_
#define _HDEF_H_


#include <stddef.h>
#include <stdint.h>

// DEF-INTERFACE
#define HFC_DYNAMICLIB
#if defined(HFC_STATICLIB) || defined(HFC_SOURCE)
  #define HFC_EXPORT
#elif defined(_MSC_VER)
  #if defined(HFC_DYNAMICLIB) || defined(HFC_EXPORTS)
      #define HFC_EXPORT __declspec(dllexport)
  #else
      #define HFC_EXPORT  __declspec(dllimport)
  #endif
#elif defined(__GNUC__)
  #define HFC_EXPORT __attribute__((visibility("default")))
#else
  #define HFC_EXPORT
#endif

// DEF-RETURN
#define HBASE_RET_OK        0x00000000
#define HBASE_RET_FILE      0x00000001
#define HBASE_RET_NOSPACE   0x00000002
#define HBASE_RET_NOTFOUND  0x00000003
#define HBASE_RET_HASEXIST  0x00000004
#define HBASE_RET_MALLOC    0x00000005
#define HBASE_RET_INIT      0x00000006
#define HBASE_RET_PARAM(N) (0x000000F0|(N))
#define HBASE_RET(BASECODE,DETAILCODE) ((BASECODE)|(DETAILCODE))

#define HBASE_RET_BASE_DS   0x00000100
#define HBASE_RET_BASE_LOG  0x00000200
#define HBASE_RET_BASE_CFG  0x00000300
#define HBASE_RET_BASE_TIME 0x00000400



//define for tool 
#define HBASE_ALLOC(S) malloc(S)
#define HBASE_FREE(P) free(P)
#define HBASE_REALLOC(P,S) realloc(P,S)
#define HBASE_GETMINPOW(V,INITV) while (INITV < V) INITV <<= 1
#define HBASE_RET_WHEN(VAL,RETCODE) if(VAL) return RETCODE



#endif