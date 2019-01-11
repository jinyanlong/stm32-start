#ifndef _RACEDATA_H
#define _RACEDATA_H

////////////////////////////////////////////////////FLASH����//////////////////////////////////////////
#define FILEADDR_SYSMAC             0x00
#define FILEADDR_SYSID              0x04
#define FILEADDR_SYSKEY             0x08


#define FILEADDR_TIMEZONE           0x10
#define FILEADDR_TAG_OFFTIME        0x12     //��ǩ���߳�ʱֵ

#define FILEADDR_TOTALSOF           0x14
#define FILEADDR_TOTALEOF           0x18
#define FILEADDR_WEB_UPLOADSOF      0x1C
#define FILEADDR_TAGPING_INTERVAL   0x20
#define FILEADDR_WEBPING_INTERVAL   0x22
#define FILEADDR_UART_UPLOADSOF     0x24
#define FILEADDR_UARTPING_INTERVAL         0x28
#define FILEADDR_UARTOVERTIME     0x2A

#define FILEADDR_SITENAME           0x40
#define FILEADDR_SERVERIP           0x80
#define FILEADDR_SERVERSERVICE      0xC0
#define FILEADDR_SERVERMETHOD       0x100


#define FILEADDR_TAGLIST     0x12000
    #define TAG_RECORDS_CAPACITY     30000   //�������ֵ32752,��һ��16��������
    #define TAG_RECORD_SIZE          32

    
#define ASTRING_TYPE_IP                  0x00
#define ASTRING_TYPE_DIR                 0x01
#define ASTRING_TYPE_APN                 0x02



//����Э��: ���(4B,�ɼ��������)+Э��汾(1B)+����(1B)[+������ID(2B)]+ѧУID(2B)+����(4B)+ʱ���׼(4B)+����(4B)+[����ID(2B)+��¼ʱ��(2B)]+CRC8
#define PROT_HEAD_OFFSET  0x04
#define PROT_BUFFER_OFFSET_VERSION     (PROT_HEAD_OFFSET+0)
#define PROT_BUFFER_OFFSET_STYLE       (PROT_HEAD_OFFSET+1)
#define PROT_BUFFER_OFFSET_TAGID       (PROT_HEAD_OFFSET+6)
#define PROT_BUFFER_OFFSET_BASETIME    (PROT_HEAD_OFFSET+10)
#define PROT_BUFFER_OFFSET_STEPER      (PROT_HEAD_OFFSET+14)
#define PROT_BUFFER_OFFSET_COINS       (PROT_HEAD_OFFSET+18)
    #define PROT_BUFFER_OFFSET_TAGID_OLD       (PROT_HEAD_OFFSET+4)
    #define PROT_BUFFER_OFFSET_BASETIME_OLD    (PROT_HEAD_OFFSET+8)
    #define PROT_BUFFER_OFFSET_STEPER_OLD      (PROT_HEAD_OFFSET+12)
    #define PROT_BUFFER_OFFSET_COINS_OLD       (PROT_HEAD_OFFSET+16)

#define TAG_PROT_VERSION       0xA0
#define TAG_PROT_STYLE_PROXY   0x01     //������ID��Ч

#define TAG_OFF_OVERTIME     12            //����10�����ϳ�ʱ�����ֱȿ�����������ʱ���Գ�һЩ

typedef struct _TAGRECORD{
    UInt8 flag;                 //���λ��ѹ����������λ��������       
    UInt8 resv;                 //����
    UInt16 coinId;
    UInt32 tagId;
    UInt32 steper;
    UInt32 saveTick;
    UInt32 raiseTick;       //��ǩ�е�ʱ��ֵ,10msΪ��λ
    Int64  baseTime;        //��ǩ0ʱ�̶�Ӧ��UTCʱ��
}TAGRECORD;


//typedef struct _TAGRECORD{
//    UInt8 flag;                  //���λ��ѹ����������λ��������       
//    UInt8 raiseTime_10ms;
//    UInt16 coinId;
//    UInt32 tagId;
//    UInt32 steper;
//    UInt32 saveTime;        //��¼����ʱ��,��������ͣ��1��������,������һ����¼startTime����,saveTime+=60��
//    UInt32 raiseTime;       //����ʱ��
//    UInt32 startBase;       //��ǩ��Ӧ��ʱ��ֵ
//}TAGRECORD;

static __INLINE Int64 TICK2UTC(Int64 baseTime,UInt32 tick_10ms){
    return baseTime+10*(Int64)tick_10ms;
}

static __INLINE void drv_Coin_toString(UInt16 coinId,char* buffer){
    sprintf(buffer,"%04X",coinId);
}
static __INLINE void drv_Tag_toString(UInt32 tagId,char* buffer){
    sprintf(buffer,"%08d",tagId);
}



void drv_Data_setSystem(UInt8* systemId,UInt32 systemMark,UInt8* pKey);
void drv_Data_getSystem(UInt8* systemId,UInt32* systemMark,UInt8* pKey);
UInt32 drv_Data_getTotalSOF(void);
UInt32 drv_Data_getTotalEOF(void);
UInt32 drv_Data_getUploadSOF(void);
UInt32 drv_Data_getUartUploadSOF(void);
void drv_Data_setTotalSOF(UInt32 value);
void drv_Data_setTotalEOF(UInt32 value);
void drv_Data_setUploadSOF(UInt32 value);
void drv_Data_setUartUploadSOF(UInt32 value);
void drv_Data_setTimeZone(Int16 zone);
Int16 drv_Data_getTimeZone(void);
void drv_Data_setSiteName(const void* pSiteName);
void drv_Data_getSiteName(UStringBuffer pSiteName);
void drv_Data_setAString(UInt8 nMode,const void* pURI);
void drv_Data_getAString(UInt8 nMode,AStringBuffer pURI);


UInt16 drv_Data_getWebPingInterval(void);
void drv_Data_setWebPingInterval(UInt16 value);
UInt16 drv_Data_getTagOfftime(void);
void drv_Data_setTagOfftime(UInt16 value);
UInt16 drv_Data_getTagPingInterval(void);
void drv_Data_setTagPingInterval(UInt16 value);

void drv_Data_setUartPingInterval(UInt16 value);
UInt16 drv_Data_getUartPingInterval(void);
void drv_Data_setUartOvertime(UInt16 value);
UInt16 drv_Data_getUartOvertime(void);
    
void drv_Data_flush(void);

void drv_Data_reset(void);
void drv_Data_clear(UInt8 nMode);
void drv_Record_set(UInt32 recordIndex,const TAGRECORD* tagItem);
void drv_Record_get(UInt32 recordIndex,TAGRECORD* tagItem);
#endif
