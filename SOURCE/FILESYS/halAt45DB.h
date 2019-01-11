#ifndef __HAL_AT45DB161B_H_001
#define __HAL_AT45DB161B_H_001

//AT45DB61是允许单页擦除的,但没有写入部分数据的指令

#define AT45DB161_PAGE_COUNT        4096
#define AT45DB161_PAGE_SIZE         528
#define AT45DB161_BLOCK_PAGECOUNT   8

#define AT45DB_EMPTY_VALUE 0xFF
#define AT45DB_PAGE_SIZE   528


bool hal_AT45DB_config(void);

//读写(内部缓冲区1)
void hal_AT45DB_readBuff1(UInt16 uAddr,void* pBuff,UInt16 bLen);
void hal_AT45DB_writeBuff1(UInt16 uAddr,const void* pBuff,UInt16 bLen);
void hal_AT45DB_fillBuff1(UInt8 nValue);
void hal_AT45DB_loadBuff1(UInt16 uPage);
void hal_AT45DB_programBuff1(UInt16 uPage);
void hal_AT45DB_flushBuff1(UInt16 uPage);


//读写(内部缓冲区1)
void hal_AT45DB_readBuff2(UInt16 uAddr,void* pBuff,UInt16 bLen);
void hal_AT45DB_writeBuff2(UInt16 uAddr,const void* pBuff,UInt16 bLen);
void hal_AT45DB_fillBuff2(UInt8 nValue);
void hal_AT45DB_loadBuff2(UInt16 uPage);
void hal_AT45DB_programBuff2(UInt16 uPage);
void hal_AT45DB_flushBuff2(UInt16 uPage);


//读(无缓冲)
void hal_AT45DB_readPage(UInt16 uPage,UInt16 uAddr,void* pDataBuf, UInt16 nlen );
bool hal_AT45DB_checkPage(UInt16 uPage,UInt16 uAddr,const void* pDataBuf, UInt16 nlen );
bool hal_AT45DB_checkPageN(UInt16 uPage,UInt16 uAddr,const void* pDataBuff, UInt16 nlen );
bool hal_AT45DB_checkValue(UInt16 uPage,UInt8 nValue);

//擦除(无缓冲)
void hal_AT45DB_erasePage(UInt16 uPage);
void hal_AT45DB_eraseBlock(UInt16 block);
void hal_AT45DB_eraseSector(UInt16 sector);




#endif

