#include "includes.h"
#include "SFile.h"
#include "cache.h"


//���ļ�ʵ��һ��������Ļ���ϵͳ,����1������ҳ��
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

//nPage=CACHEPAGE_NONEʱ,��ʾ����һ�����л���
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
