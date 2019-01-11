#ifndef __CASHFILE_H_001
#define __CASHFILE_H_001

#include "cache.h"

void drv_CacheFile_init(const SFILE* pThis);


void drv_CacheFile_open(const SFILE* pThis);
void drv_CacheFile_close(const SFILE* pThis);
void drv_CacheFile_read(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,void* pData,UInt16 nLen );
void drv_CacheFile_write(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,const void* pData,UInt16 nLen);
void drv_CacheFile_flush(const SFILE* pThis);
void drv_CacheFile_format(const SFILE* pThis,FILEFORMAT_CALLBACK pFunc);

//Î»·´¶ÁÐ´
#define drv_CacheFile_openN drv_CacheFile_open
#define drv_CacheFile_closeN drv_CacheFile_close
void drv_CacheFile_readN(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,void* pData,UInt16 nLen );
void drv_CacheFile_writeN(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,const void* pData,UInt16 nLen);
void drv_CacheFile_flushN(const SFILE* pThis);

#endif

