#include "includes.h"
#include "SFile.h"
#include "cacheFile.h"
#include "halAT45DB.h"
#include "halCPU.h"
//���ļ�ʵ��һ���򵥵��ļ�,�߼�ҳ��(FilePage)������ҳ��(NandPage)һһ��Ӧ

/////////////////////CashFile�Ĺ��ܺ���////////////////
void drv_CacheFile_init(const SFILE* pThis){
    static UInt8 _cacheBuffer[NAND_PAGE_SIZE]; //������������һ������ҳ���С
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
        if( (pThis->pCache->page%NAND_SECTOR_PAGECOUNT)==0 ){//pThis->pCache->page ��SECOTR֮��ҳ����ִ��һ�β�������
            hal_Flash_eraseSector( (pThis->nandStartPage+pThis->pCache->page)/NAND_SECTOR_PAGECOUNT );
        }
               
        if(!hal_Flash_writePage(pThis->nandStartPage+pThis->pCache->page,pThis->pCache->buffer)){
            drv_Exception_throw(ERROR_FLASH);
        }
        pThis->pCache->dirty=false;
    }
}

//������(ע�����ﲻ�ܸ���д����,����һ�㶼��ֱ�Ӷ�FLASHy)
void drv_CacheFile_read(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,void* pData,UInt16 nLen){
    short cashIndex;
    cashIndex=hal_Cache_find(pThis->pCache,nPage);
    if(cashIndex<0){
        //ֱ�Ӷ�ȡ����ҳ��(�����뻺����)
        hal_Flash_readPage(FILE_NANDPAGE_TOAINDEX(nPage),nAddr,pData,nLen);
    }else{
        hal_Cache_read(pThis->pCache,nAddr,pData,nLen);
    }
}

//д����
void drv_CacheFile_write(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,const void* pData,UInt16 nLen){
    short cashIndex;
    cashIndex=hal_Cache_find(pThis->pCache,nPage);
    if( cashIndex<0 ){
        drv_CacheFile_flush(pThis);
        cashIndex=0;
        
        //���������ݶ��뻺��
        hal_Flash_readPage(FILE_NANDPAGE_TOAINDEX(nPage),0,pThis->pCache->buffer,NAND_PAGE_SIZE); // ֻ��ȡ�߼�ҳ��
        pThis->pCache->page=nPage;
        pThis->pCache->dirty=false;
    }
    hal_Cache_write(pThis->pCache,nAddr,pData,nLen);
}



void drv_CacheFile_format(const SFILE* pThis,FILEFORMAT_CALLBACK pFunc){
    UInt16 i;
    
    if(pFunc) pFunc(pThis,FILE_FORMATFLAG_BEFORE,-1);
    for(i=0;i< (pThis->nandPageCount/NAND_SECTOR_PAGECOUNT);i++){
        hal_IDOG_flush(); //��ʽ��ʱ��ϳ�,��Ҫˢ��
        if(pFunc) pFunc(pThis,FILE_FORMATFLAG_BEFORE,i);
        hal_Flash_eraseSector(FILE_NANDSECTOR_TOAINDEX(i));
        if(pFunc) pFunc(pThis,FILE_FORMATFLAG_AFTER,i);
    }
    if(pFunc) pFunc(pThis,FILE_FORMATFLAG_AFTER,-1);
    
    drv_CacheFile_open(pThis);
}



////������
//void drv_CacheFile_readN(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,void* pData,UInt16 nLen){
//    short cashIndex;
//    cashIndex=hal_Cache_find(pThis->pCache,nPage);
//    if(cashIndex<0){
//        //ֱ�Ӷ�ȡ����ҳ��(�����뻺����)
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
//        //���������ݶ��뻺��
//        hal_Flash_readPageN(FILE_NANDPAGE_TOAINDEX(nPage),0,pThis->pCache->buffer,NAND_PAGE_SIZE); // ֻ��ȡ�߼�ҳ��
//        pThis->pCache->page=nPage;
//        pThis->pCache->dirty=false;
//    }
//    hal_Cache_write(pThis->pCache,nAddr,pData,nLen);
//}

//void drv_CacheFile_flushN(const SFILE* pThis){
//    if( (pThis->pCache->page >= 0) && (pThis->pCache->dirty) ){
//        if( (pThis->pCache->page%NAND_SECTOR_PAGECOUNT)==0 ){//pThis->pCache->page ��SECOTR֮��ҳ����ִ��һ�β�������
//            hal_Flash_eraseSector( (pThis->nandStartPage+pThis->pCache->page)/NAND_SECTOR_PAGECOUNT );
//        }

//        if(!hal_Flash_writePageN(pThis->nandStartPage+pThis->pCache->page,pThis->pCache->buffer)){
//            drv_Exception_throw(ERROR_FLASH);
//        }
//        pThis->pCache->dirty=false;
//    }
//}
