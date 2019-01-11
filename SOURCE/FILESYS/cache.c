#include "includes.h"
#include "SFile.h"
#include "cache.h"


//本文件实现一个最基本的缓冲系统,缓冲1个物理页面
void hal_Cache_init(CACHESTRUCT* pThis,UInt8* buffer,int bufferSize){
    pThis->buffer=buffer;
    pThis->page=CACHEPAGE_NONE;
    pThis->dirty=false;
    memset(buffer,0,bufferSize);
}

void hal_Cache_reset(CACHESTRUCT* pThis){
    pThis->page=CACHEPAGE_NONE;
    pThis->dirty=false;
}


UInt8* hal_Cache_get(CACHESTRUCT* pThis){
    return pThis->buffer;
}

//nPage=CACHEPAGE_NONE时,表示查找一个空闲缓冲
short hal_Cache_find(CACHESTRUCT* pThis,short nPage){
    if( pThis->page==nPage ){
        return 0;
    }

    return -1;
}
void drv_CacheFile_dumpPage(CACHESTRUCT* pThis,short nPage){
    short index;
    index=hal_Cache_find(pThis,nPage);
    if(index>=0){
        pThis->page=nPage;
        pThis->dirty=false;
    }
}
void hal_Cache_read(CACHESTRUCT* pThis,UInt16 nAddr,UInt8* pData,UInt16 nLen){
    memcpy(pData,&pThis->buffer[nAddr],nLen);
}
void hal_Cache_write(CACHESTRUCT* pThis,UInt16 nAddr,const void* pData,UInt16 nLen){
    memcpy(&pThis->buffer[nAddr],pData,nLen);
    pThis->dirty=true;
}
