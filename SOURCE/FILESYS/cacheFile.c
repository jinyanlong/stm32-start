#include "includes.h"
#include "SFile.h"
#include "cacheFile.h"
#include "halAT45DB.h"
#include "halCPU.h"
//本文件实现一个简单的文件,逻辑页面(FilePage)与物理页面(NandPage)一一对应

/////////////////////CashFile的功能函数////////////////
void drv_CacheFile_init(const SFILE* pThis){
    static UInt8 _cacheBuffer[NAND_PAGE_SIZE]; //缓冲区必须有一个物理页面大小
    hal_Cache_init(pThis->pCache,_cacheBuffer,NAND_PAGE_SIZE);
}



void drv_CacheFile_open(const SFILE* pThis){
    hal_Cache_reset(pThis->pCache);
}
void drv_CacheFile_close(const SFILE* pThis){
    pThis->flush(pThis);
}

void drv_CacheFile_flush(const SFILE* pThis){           
    if( (pThis->pCache->page >= 0) && (pThis->pCache->dirty) ){
        if( (pThis->pCache->page%NAND_SECTOR_PAGECOUNT)==0 ){//pThis->pCache->page 是SECOTR之首页，就执行一次擦除操作
            hal_Flash_eraseSector( (pThis->nandStartPage+pThis->pCache->page)/NAND_SECTOR_PAGECOUNT );
        }
               
        if(!hal_Flash_writePage(pThis->nandStartPage+pThis->pCache->page,pThis->pCache->buffer)){
            drv_Exception_throw(ERROR_FLASH);
        }
        pThis->pCache->dirty=false;
    }
}

//读数据(注意这里不能干扰写操作,所以一般都是直接读FLASHy)
void drv_CacheFile_read(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,void* pData,UInt16 nLen){
    short cashIndex;
    cashIndex=hal_Cache_find(pThis->pCache,nPage);
    if(cashIndex<0){
        //直接读取物理页面(不读入缓冲区)
        hal_Flash_readPage(FILE_NANDPAGE_TOAINDEX(nPage),nAddr,pData,nLen);
    }else{
        hal_Cache_read(pThis->pCache,nAddr,pData,nLen);
    }
}

//写数据
void drv_CacheFile_write(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,const void* pData,UInt16 nLen){
    short cashIndex;
    cashIndex=hal_Cache_find(pThis->pCache,nPage);
    if( cashIndex<0 ){
        drv_CacheFile_flush(pThis);
        cashIndex=0;
        
        //将物理数据读入缓冲
        hal_Flash_readPage(FILE_NANDPAGE_TOAINDEX(nPage),0,pThis->pCache->buffer,NAND_PAGE_SIZE); // 只读取逻辑页面
        pThis->pCache->page=nPage;
        pThis->pCache->dirty=false;
    }
    hal_Cache_write(pThis->pCache,nAddr,pData,nLen);
}



void drv_CacheFile_format(const SFILE* pThis,FILEFORMAT_CALLBACK pFunc){
    UInt16 i;
    
    if(pFunc) pFunc(pThis,FILE_FORMATFLAG_BEFORE,-1);
    for(i=0;i< (pThis->nandPageCount/NAND_SECTOR_PAGECOUNT);i++){
        hal_IDOG_flush(); //格式化时间较长,需要刷狗
        if(pFunc) pFunc(pThis,FILE_FORMATFLAG_BEFORE,i);
        hal_Flash_eraseSector(FILE_NANDSECTOR_TOAINDEX(i));
        if(pFunc) pFunc(pThis,FILE_FORMATFLAG_AFTER,i);
    }
    if(pFunc) pFunc(pThis,FILE_FORMATFLAG_AFTER,-1);
    
    drv_CacheFile_open(pThis);
}



////读数据
//void drv_CacheFile_readN(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,void* pData,UInt16 nLen){
//    short cashIndex;
//    cashIndex=hal_Cache_find(pThis->pCache,nPage);
//    if(cashIndex<0){
//        //直接读取物理页面(不读入缓冲区)
//        hal_Flash_readPageN(FILE_NANDPAGE_TOAINDEX(nPage),nAddr,pData,nLen);
//    }else{
//        hal_Cache_read(pThis->pCache,nAddr,pData,nLen);
//    }
//}

//void drv_CacheFile_writeN(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,const void* pData,UInt16 nLen){
//    short cashIndex;
//    cashIndex=hal_Cache_find(pThis->pCache,nPage);
//    if( cashIndex<0 ){
//        drv_CacheFile_flushN(pThis);
//        cashIndex=0;
//        
//        //将物理数据读入缓冲
//        hal_Flash_readPageN(FILE_NANDPAGE_TOAINDEX(nPage),0,pThis->pCache->buffer,NAND_PAGE_SIZE); // 只读取逻辑页面
//        pThis->pCache->page=nPage;
//        pThis->pCache->dirty=false;
//    }
//    hal_Cache_write(pThis->pCache,nAddr,pData,nLen);
//}

//void drv_CacheFile_flushN(const SFILE* pThis){
//    if( (pThis->pCache->page >= 0) && (pThis->pCache->dirty) ){
//        if( (pThis->pCache->page%NAND_SECTOR_PAGECOUNT)==0 ){//pThis->pCache->page 是SECOTR之首页，就执行一次擦除操作
//            hal_Flash_eraseSector( (pThis->nandStartPage+pThis->pCache->page)/NAND_SECTOR_PAGECOUNT );
//        }

//        if(!hal_Flash_writePageN(pThis->nandStartPage+pThis->pCache->page,pThis->pCache->buffer)){
//            drv_Exception_throw(ERROR_FLASH);
//        }
//        pThis->pCache->dirty=false;
//    }
//}
