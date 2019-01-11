#ifndef __DATAFILE_H_001
#define __DATAFILE_H_001

#include "cache.h"
#include "halAt45DB.h"
#include "SFile.h"
#include "RealTimer.h"


//FLASH �е������Ѿ����ã�����0��ʾ�գ�1��ʾ�ѱ��
#define PAGEVALUE_EMPTY     0

#define NAND_PM_BAD    0x3       //����(��ϵͳ��������������,����Ϊ����ͨ���������޸�)
#define NAND_PM_VALID  0x1       //��ʹ�ã��ǿ�
#define NAND_PM_DUMP   0x2       //������,�ǿ�
#define NAND_PM_EMPTY  0x0       //�հ�ҳ



#define FILE_PM_VALID  1       //�߼�ҳ����Ч
#define FILE_PM_NONE   0       //�߼�ҳ����Ч


//�߼�ҳ����Ϣ��(2�ֽ�)
typedef UInt16 DATAFILE_PAGEMAP;

//��Ч��Ч,2λ��ʾ
#define PM_SETFLAG(pm,flag)          do{(pm)=((pm)& 0x3FFF) | (((flag)&0x03)<<14);}while(0)
#define PM_GETFLAG(pm)               ((pm)>>14)

//�����ַ
#define PM_SETINDEX(pm,pageIndex)    do{(pm)=((pm)& 0xC000) | ((pageIndex)&0x3FFF);}while(0)
#define PM_GETINDEX(pm)              ((pm)&0x3FFF)

#define FPM_CREATE(flag,sectorIndex)              ((((flag)&0x03)<<14)|(sectorIndex))

//����ҳ�渽����Ϣ(8B),��ҳ��BANDȫ0xFF,�հ�ҳ��BANDȫ0,
typedef struct _PAGEBAND{
    UInt32 timeTag;              //4B(��ˮʱ�������Զ����,�ļ���ȫ��Ψһ,�������ж��ĸ�ҳ�������µ�)
    UInt32 sectorTag;            //�������ƶ�ʱ��ʱ���(Ĭ��ֵ��0, �������ƶ�ʱ,sectorTag++)
    UInt16 npm;                  //2bits��Ч��־λ,14bits�ļ��߼�ҳ��
    UInt16 bandEcc;         //�����ݽṹ��CRCУ��
}PAGEBAND;



#define BND_SETFLAG(band,flag)        PM_SETFLAG(band.npm,flag)
#define BND_GETFLAG(band)             PM_GETFLAG(band.npm)
//#define BND_SWITCHTIMETAG(band)       do{band.timeTag++;}while(0)
#define BND_SETTIMETAG(band,timeTag)  do{band.timeTag=timeTag;}while(0)
#define BND_GETTIMETAG(band)          (band.timeTag)
#define BND_SETPAGE(band,page)        PM_SETINDEX(band.npm,page)        //�����ļ�ҳ��
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
    UInt8* NPM;                              //����ҳ(NANDPAGE)��: �������ҳ��״̬(�հ�/ʹ��/����/ʧЧ)
    DATAFILE_PAGEMAP* FPM;                   //�߼�ҳ(FILEPAGE)��:FPM[�߼�ҳ��]=����ҳ��Ϣ��2b��Ч��Ϣ+14b����ҳ�ţ�
    short nandSOF;                           //��һ���ɷ�������ҳ��Ŀ�ʼλ��
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

