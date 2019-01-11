#ifndef __SFILE_H_001
#define __SFILE_H_001

#include "halAt45DB.h"
#include "cache.h"

//////////////////////////////////////��ֲ����Ҫ�ĳ���������////////////////////////////////////
#define NAND_CHIP_PAGECOUNT      AT45DB161_PAGE_COUNT           //�����õ�����ҳ������
#define NAND_SECTOR_PAGECOUNT    AT45DB161_BLOCK_PAGECOUNT      //��С�ɲ�������,�ر�ע��:����AT45DB161��˵,��С����1ҳ
#define NAND_PAGE_SIZE           AT45DB161_PAGE_SIZE            //һ��ҳ�������ߴ�


//����հ����������ޣ�ֱ�ӷ���հ���
//����հ����������ޣ�����ִ��һ�λ��ղ�����ֱ���հ����ص�����
#define EMPTY_SECTORCOUNT_MIN    3    //�հ�������(����������������������,�����޷�����)
#define EMPTY_SECTORCOUNT_RESV   8    //�����Ŀհ���,����>=EMPTY_SECTORCOUNT_MIN
#define FILE_PAGE_SIZE          512    //�߼�ҳ��ߴ�

///////////////////////////////////����ʵ���ļ�������////////////////////////////////

#define FONTFILE_STARTPAGE          0
#define FONTFILE_NAND_PAGECOUNT     1016
#define FONTFILE_PAGECOUNT          (FONTFILE_NAND_PAGECOUNT-0*NAND_SECTOR_PAGECOUNT)

#define DATAFILE_STARTPAGE          1024
#define DATAFILE_NAND_PAGECOUNT     3072  //((4+EMPTY_SECTORCOUNT_RESV)*NAND_SECTOR_PAGECOUNT)
#define DATAFILE_PAGECOUNT          (DATAFILE_NAND_PAGECOUNT-EMPTY_SECTORCOUNT_RESV*NAND_SECTOR_PAGECOUNT)    // �۳��������� > EMPTY_SECTORCOUNT_MIN, ����Ĳ��־�������Ļ���

#define PAGE_INDEX_INVALID           (-1)
#define SECTOR_INDEX_INVALID         (-1)

//�������������ת���������������
#define FILE_NANDPAGE_TOAINDEX(pageRIndex)          (pageRIndex+FILE_PAGE_NANDSTART)
#define FILE_NANDSECTOR_TOAINDEX(sectorRIndex)      (sectorRIndex+FILE_PAGE_NANDSTART/NAND_SECTOR_PAGECOUNT)

#define FILE_PAGE_COUNT              pThis->filePageCount
#define FILE_PAGE_NANDSTART          pThis->nandStartPage          //�ļ���,��ʼ����ҳ��
#define FILE_PAGE_NANDCOUNT          pThis->nandPageCount          //�ļ���,ʵ��ռ�õ�����ҳ������
#define FILE_SECTOR_NANDCOUNT        (FILE_PAGE_NANDCOUNT/NAND_SECTOR_PAGECOUNT)   //�ļ���,������������
//#define FILE_SECTOR_RESERVED        ((FILE_PAGE_NANDCOUNT-FILE_PAGE_COUNT)/NAND_SECTOR_PAGECOUNT)            //����������
         


#define FLASH_FLAG_SECTOR   (1016/8)
#define FLASH_FONT_PAGE   1016
#define FLASH_DATA_PAGE   1017
#define FLASH_FLAG_FORMAT 0x12345678
#define FLASH_FLAG_FONTNORMAL 0xA5A5A5A5
#define FLASH_FLAG_DATANORMAL 0x18120603


void hal_Flash_init(void);
void hal_Flash_readPage(UInt16 pageIndex,UInt16 uAddr,UInt8* pageBuff,UInt16 nSize);
void hal_Flash_readPageN(UInt16 pageIndex,UInt16 uAddr,UInt8* pageBuff,UInt16 nSize);
void hal_Flash_dumpPage(UInt16 pageIndex);
bool hal_Flash_writePage(UInt16 pageIndex,UInt8* pageBuff);
bool hal_Flash_writePageN(UInt16 pageIndex,UInt8* pageBuff);
void hal_Flash_eraseSector(UInt16 sectorIndex);
bool hal_Flash_checkPage(UInt16 pageIndex,UInt8 value);
bool hal_Flash_checkPageN(UInt16 pageIndex,UInt8 value);


void hal_Flash_setFlag(UInt16 pageIndex,UInt32 flag);
UInt32 hal_Flash_getFlag(UInt16 pageIndex);

    
#define FILE_FORMATFLAG_BEFORE 0x01
#define FILE_FORMATFLAG_AFTER  0x02

typedef const struct _SFILE* PSFILE;
typedef  void (*FILEFORMAT_CALLBACK)( PSFILE pFile,int flag,int block);
typedef UInt32 FILE_HEAD;
typedef struct _SFILE{
    short nandStartPage;           //����ҳ����ʼλ��
    short nandPageCount;           //����ҳ������,������NAND_SECTOR_PAGECOUNT��������
    short filePageCount;
    CACHESTRUCT* pCache;
    void (*init)(const struct _SFILE* fp);
    void (*open)(const struct _SFILE* fp);
    void (*close)(const struct _SFILE* fp);
    void (*read)(const struct _SFILE* fp,UInt16 nPage,UInt16 nAddr,void* pData,UInt16 nLen );
    void (*write)(const struct _SFILE* fp,UInt16 nPage,UInt16 nAddr,const void* pData,UInt16 nLen);
    void (*flush)(const struct _SFILE* fp);
    void (*format)(const struct _SFILE* fp,FILEFORMAT_CALLBACK pFunc);
}SFILE;




extern const SFILE s_dataFile;
extern const SFILE s_fontFile;

void drv_File_open(const SFILE* fp);
void drv_File_close(const SFILE* fp);
void drv_File_format(const SFILE* fp,FILEFORMAT_CALLBACK pFunc);
void drv_File_write(const SFILE* fp,UInt32 nAddr,const void* pBuff,UInt32 nLen);
void drv_File_read(const SFILE* fp,UInt32 nAddr,void* pData,UInt32 nLen );
void drv_File_flush(const SFILE* fp);


void fns_File_open(const SFILE* fp);
void fns_File_close(const SFILE* fp);
void fns_File_format(const SFILE* fp,FILEFORMAT_CALLBACK pFunc);
void fns_File_write(const SFILE* fp,UInt32 nAddr,const void* pBuff,UInt32 nLen);
void fns_File_read(const SFILE* fp,UInt32 nAddr,void* pData,UInt32 nLen );
void fns_File_flush(const SFILE* fp);


void drv_File_init(void);
void drv_File_doFormat(const SFILE* fp,FILEFORMAT_CALLBACK callback);
void drv_File_startNormal(void);
#endif

