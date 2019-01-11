#ifndef __CACHE_H_001
#define __CACHE_H_001

#define CACHEPAGE_NONE -1
typedef struct _CACHESTRUCT{
    UInt8* buffer;     //当前版本仅对最近一个页面进行缓冲
    short  page;       //缓冲区中的逻辑页码
    bool  dirty;       //缓冲区是否Dirty
}CACHESTRUCT;

void hal_Cache_init(CACHESTRUCT* pThis,UInt8* buffer,int bufferSize);
void hal_Cache_reset(CACHESTRUCT* pThis);
UInt8* hal_Cache_get(CACHESTRUCT* pThis);
short hal_Cache_find(CACHESTRUCT* pThis,short nPage);
void hal_Cache_read(CACHESTRUCT* pThis,UInt16 nAddr,UInt8* pData,UInt16 nLen);
void hal_Cache_write(CACHESTRUCT* pThis,UInt16 nAddr,const void* pData,UInt16 nLen);
void drv_CacheFile_dumpPage(CACHESTRUCT* pThis,short nPage);


#endif

