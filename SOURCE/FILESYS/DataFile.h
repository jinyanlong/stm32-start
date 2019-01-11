#ifndef __DATAFILE_H_001
#define __DATAFILE_H_001

#include "cache.h"
#include "halAt45DB.h"
#include "SFile.h"
#include "RealTimer.h"


//FLASH 中的数据已经反置，所以0表示空，1表示已编程
#define PAGEVALUE_EMPTY     0

#define NAND_PM_BAD    0x3       //已损坏(本系统不考虑物理性损坏,总认为可以通过擦除来修复)
#define NAND_PM_VALID  0x1       //已使用，非空
#define NAND_PM_DUMP   0x2       //已作废,非空
#define NAND_PM_EMPTY  0x0       //空白页



#define FILE_PM_VALID  1       //逻辑页面有效
#define FILE_PM_NONE   0       //逻辑页面无效


//逻辑页面信息表(2字节)
typedef UInt16 DATAFILE_PAGEMAP;

//有效无效,2位表示
#define PM_SETFLAG(pm,flag)          do{(pm)=((pm)& 0x3FFF) | (((flag)&0x03)<<14);}while(0)
#define PM_GETFLAG(pm)               ((pm)>>14)

//物理地址
#define PM_SETINDEX(pm,pageIndex)    do{(pm)=((pm)& 0xC000) | ((pageIndex)&0x3FFF);}while(0)
#define PM_GETINDEX(pm)              ((pm)&0x3FFF)

#define FPM_CREATE(flag,sectorIndex)              ((((flag)&0x03)<<14)|(sectorIndex))

//物理页面附加信息(8B),坏页的BAND全0xFF,空白页的BAND全0,
typedef struct _PAGEBAND{
    UInt32 timeTag;              //4B(流水时间戳，永远递增,文件内全局唯一,可用于判断哪个页面是最新的)
    UInt32 sectorTag;            //数据区移动时的时间戳(默认值是0, 当区块移动时,sectorTag++)
    UInt16 npm;                  //2bits有效标志位,14bits文件逻辑页面
    UInt16 bandEcc;         //本数据结构的CRC校验
}PAGEBAND;



#define BND_SETFLAG(band,flag)        PM_SETFLAG(band.npm,flag)
#define BND_GETFLAG(band)             PM_GETFLAG(band.npm)
//#define BND_SWITCHTIMETAG(band)       do{band.timeTag++;}while(0)
#define BND_SETTIMETAG(band,timeTag)  do{band.timeTag=timeTag;}while(0)
#define BND_GETTIMETAG(band)          (band.timeTag)
#define BND_SETPAGE(band,page)        PM_SETINDEX(band.npm,page)        //设置文件页码
#define BND_GETPAGE(band)             PM_GETINDEX(band.npm)

#define PBND_SETFLAG(pband,flag)       PM_SETFLAG((pband)->npm,flag)
#define PBND_GETFLAG(pband)            PM_GETFLAG((pband)->npm)
//#define PBND_SWITCHTIMETAG(pband)      do{(pband)->timeTag++;}while(0)
#define PBND_SETTIMETAG(pband,value) do{(pband)->timeTag=value;}while(0)
#define PBND_GETTIMETAG(pband)         ((pband)->timeTag)
#define PBND_SETPAGE(pband,page)       PM_SETINDEX((pband)->npm,page)
#define PBND_GETPAGE(pband)            PM_GETINDEX((pband)->npm)


typedef struct _DATACACHE{
    CACHESTRUCT cache;
    UInt8* NPM;                              //物理页(NANDPAGE)表: 存放物理页面状态(空白/使用/作废/失效)
    DATAFILE_PAGEMAP* FPM;                   //逻辑页(FILEPAGE)表:FPM[逻辑页号]=物理页信息（2b有效信息+14b物理页号）
    short nandSOF;                           //下一个可分配物理页面的开始位置
    UInt32 maxTimeTag;
} DATACACHE;


UInt32 drv_DataFile_getFileSize(const SFILE* pThis);
void drv_DataFile_init(const SFILE* pThis);
void drv_DataFile_openN(const SFILE* pThis);
void drv_DataFile_close(const SFILE* pThis);
void drv_DataFile_readN(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,void* pData,UInt16 nLen );
void drv_DataFile_writeN(const SFILE* pThis,UInt16 nPage,UInt16 nAddr,const void* pData,UInt16 nLen);
void drv_DataFile_flushN(const SFILE* pThis);
void drv_DataFile_format(const SFILE* pThis,FILEFORMAT_CALLBACK pFunc);

#define drv_DataFile_closeN  drv_DataFile_close
#define drv_DataFile_formatN drv_DataFile_format


#endif

