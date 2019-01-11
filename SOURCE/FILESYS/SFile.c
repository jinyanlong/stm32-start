#include "includes.h"  
#include "halCPU.h"
#include "SFile.h"
#include "cacheFile.h"
#include "DataFile.h"

static CACHESTRUCT s_FontFile_cache;
    static UInt8 _fontBuffer[NAND_PAGE_SIZE]; //缓冲区必须有一个物理页面大小
static DATACACHE s_DataFile_cache;
    static UInt8 _npmBuffer[DATAFILE_NAND_PAGECOUNT/4];         //物理页表: 存放页面状态(空白/使用/失效)
    static DATAFILE_PAGEMAP _fpmBuffer[DATAFILE_PAGECOUNT];    //逻辑页表: 存放对应的物理页面索引信息
    static UInt8 _dataBuffer[NAND_PAGE_SIZE]; //缓冲区必须有一个物理页面大小


const SFILE s_fontFile={
    FONTFILE_STARTPAGE,
    FONTFILE_NAND_PAGECOUNT,//页面数量一定要是SECTOR的整数倍
    FONTFILE_PAGECOUNT,
    &s_FontFile_cache,
    drv_CacheFile_init,
    drv_CacheFile_open,
    drv_CacheFile_close,
    drv_CacheFile_read,    //为保持和以前兼容，没有用反操作
    drv_CacheFile_write,   //为保持和以前兼容，没有用反操作
    drv_CacheFile_flush,
    drv_CacheFile_format
};

const SFILE s_dataFile={
    DATAFILE_STARTPAGE,
    DATAFILE_NAND_PAGECOUNT,
    DATAFILE_PAGECOUNT,
    (CACHESTRUCT*)&s_DataFile_cache,
    drv_DataFile_init,
    drv_DataFile_openN,
    drv_DataFile_closeN,
    drv_DataFile_readN,     //为简化编程,用反操作
    drv_DataFile_writeN,    //为简化编程,用反操作
    drv_DataFile_flushN,
    drv_DataFile_formatN
};


//返回文件最大逻辑尺寸
UInt32 drv_File_getMaxSize(const SFILE* pThis){
    return (pThis->filePageCount*FILE_PAGE_SIZE);
}

void drv_File_open(const SFILE* pThis){
    pThis->open(pThis);
}

void drv_File_close(const SFILE* pThis){
    pThis->close(pThis);
}


//格式化
void drv_File_format(const SFILE* pThis,FILEFORMAT_CALLBACK pFunc){    
    pThis->format(pThis,pFunc);
}

void drv_File_write(const SFILE* pThis,UInt32 nAddr,const void* pBuff,UInt32 nLen){
    UInt16 nFirstOffset,nLastOffset,i;
    UInt8* pData=(UInt8*)pBuff;
    
    if( (nAddr+nLen) >  drv_File_getMaxSize(pThis) ){
        char desc[64];
        sprintf(desc,"write(%X,%X)",nAddr,nLen);
        drv_Exception_throwEx(ERROR_FILE_WOVERSIZE,0,desc);

    }
    if(nLen==0) return;
    
    nFirstOffset=nAddr%FILE_PAGE_SIZE;
    
    //单页写处理
    if( (nFirstOffset+ nLen)<= FILE_PAGE_SIZE ){
        pThis->write(pThis,nAddr/FILE_PAGE_SIZE,nFirstOffset,pData,nLen);
        return;
    }
    
    //首页
    pThis->write(pThis,nAddr/FILE_PAGE_SIZE,nFirstOffset,pData,FILE_PAGE_SIZE-nFirstOffset);
    pData+=FILE_PAGE_SIZE-nFirstOffset;

    //中间页
    for(i=nAddr/FILE_PAGE_SIZE+1;i<(nAddr+nLen)/FILE_PAGE_SIZE;i++){
        pThis->write(pThis,i,0,pData,FILE_PAGE_SIZE);
        pData+=FILE_PAGE_SIZE;
    }

    //尾页
    nLastOffset=(nAddr+nLen)%FILE_PAGE_SIZE;
    if(nLastOffset!=0){
        pThis->write(pThis,i,0,pData,nLastOffset);
    }
}



void drv_File_read(const SFILE* pThis,UInt32 nAddr,void* pData,UInt32 nLen ){
    UInt16 i;
    UInt16 nFirstOffset;
    UInt16 nLastOffset;
    
    if( (nAddr+nLen) >  drv_File_getMaxSize(pThis) ){
        char desc[64];
        if(&s_fontFile==pThis) return; //字库文件出错,可忽略
        
        sprintf(desc,"read(%X,%X)",nAddr,nLen);
        drv_Exception_throwEx(ERROR_FILE_ROVERSIZE,0,desc);
    }
    if(nLen==0) return;

    nFirstOffset=nAddr%FILE_PAGE_SIZE;
    
    //单页写处理 执行
    if( (nFirstOffset+ nLen)<= FILE_PAGE_SIZE ){
        pThis->read(pThis,nAddr/FILE_PAGE_SIZE,nFirstOffset,pData,nLen);
        return;
    }
    
    //首页
    pThis->read(pThis,nAddr/FILE_PAGE_SIZE,nFirstOffset,pData,FILE_PAGE_SIZE-nFirstOffset);
    pData=(UInt8*)pData+FILE_PAGE_SIZE-nFirstOffset;


    //中间页
    for(i=nAddr/FILE_PAGE_SIZE+1;i<(nAddr+nLen)/FILE_PAGE_SIZE;i++){
        pThis->read(pThis,i,0,pData,FILE_PAGE_SIZE);
        pData=(UInt8*)pData+FILE_PAGE_SIZE;
    }

    //尾页
    nLastOffset=(nAddr+nLen)%FILE_PAGE_SIZE;
    if(nLastOffset!=0){
        pThis->read(pThis,i,0,pData,nLastOffset);
    }
}



void drv_File_flush(const SFILE* fp){
    fp->flush(fp);
}


////////////////////////////////////////////////////////////////////////////
void fns_File_open(const SFILE* pThis){
    FLASH_LOCK();
    drv_File_open(pThis);
    FLASH_UNLOCK();
}
void fns_File_close(const SFILE* pThis){
    FLASH_LOCK();
    drv_File_close(pThis);
    FLASH_UNLOCK();
}
void fns_File_read(const SFILE* pThis,UInt32 nAddr,void* pData,UInt32 nLen ){
    FLASH_LOCK();
    drv_File_read(pThis,nAddr,pData,nLen);
    FLASH_UNLOCK();
}

void fns_File_format(const SFILE* pThis,FILEFORMAT_CALLBACK pFunc){
    FLASH_LOCK();
    drv_File_format(pThis,pFunc);
    FLASH_UNLOCK();
}
void fns_File_write(const SFILE* pThis,UInt32 nAddr,const void* pBuff,UInt32 nLen){
    FLASH_LOCK();
    drv_File_write(pThis,nAddr,pBuff,nLen);
    FLASH_UNLOCK();
}
void fns_File_flush(const SFILE* pThis){
    FLASH_LOCK();
    drv_File_flush(pThis);
    FLASH_UNLOCK();
}

//文件系统初始化
void drv_File_init(void){
    hal_Flash_init();
    
    //因为内存需要静态分配,所以只能在这里显示调用hal_Cache_init
    hal_Cache_init(&s_FontFile_cache,_fontBuffer,sizeof(_fontBuffer));
    s_fontFile.init(&s_fontFile);

    hal_Cache_init((CACHESTRUCT*)&s_DataFile_cache,_dataBuffer,sizeof(_dataBuffer));
    s_DataFile_cache.NPM=_npmBuffer;                         
    s_DataFile_cache.FPM=_fpmBuffer;
    s_dataFile.init(&s_dataFile);
}



void drv_File_startNormal(void){
    s_dataFile.open(&s_dataFile);
}




