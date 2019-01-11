#ifndef __RECORDLIST_H
#define __RECORDLIST_H

#include "SFTypes.h"

#include "RaceData.h"

#define COIN_STATE_MASK                     0x03
    #define COIN_STATE_ONLINE               0x00        //����(�ѱ����¼)
    #define COIN_STATE_OFFLINE              0x01        //����(�ѱ����¼)
    #define COIN_STATE_OFFLINING            0x02        //����(��δ�����¼)
#define TAG_FLAG_MASK                       0x80
    #define TAG_FLAG_LOWPOWER               0x80        //�͵�ѹ��������flag�����λ

//����ͨѶ�ӳٵĴ���,�ɼ���ʱ�����ǩʱ�䲢���ܻ�����,������Ҫ�ñ�ǩTick��У׼
//�����Tick�����Ա�ǩTickΪ��׼, ��ʵ��ʱ��=(baseTime+tick*10)
typedef struct _TAGCACHE{
    Int64 baseTime;       //��ǩ0ʱ�̶�Ӧ��UTCʱ��(��ȫ�ַ�Χ���������Ǻܾ�ȷ����"ͬһ�ֽ���"��׼ȷ��)
    
    UInt8 state;            //״̬ ���λ1Ϊ�͵�ѹ����2λ��ʾ��������
    UInt16 coinId;          //coinId��ONLINE��־

    UInt32 tagId;    
    UInt32 startTick;   //��ʼʱ��(��),��Ҫ��;����activeTimeһ��У׼ʱ��
    UInt32 activeTick;  //����ʱ��(10msΪ��λ)
    UInt32 offTick;     //�뿪ʱ��(10msΪ��λ)
}TAGCACHE;


//typedef struct _TAGCACHE{    
//    UInt8 state;            //״̬ ���λ1Ϊ�͵�ѹ����2λ��ʾ��������
//    UInt8 startTime_10ms;
//    UInt8 activeTime_10ms;
//    UInt8 offTime_10ms;
//    
//    UInt32 recordTime;  //���һ�μ�¼���߱���ʱ��
//    
//    UInt32 startTime;   //��ʼʱ��(��),��Ҫ��;����startBaseһ��У׼ʱ��
//    UInt32 activeTime;  //����ʱ��(��)
//    UInt32 offTime;     //�뿪ʱ��(��)

//    UInt32 tagId;
//    UInt16 coinId;          //coinId��ONLINE��־
//}TAGCACHE;

#define CACHE_CAPACITY     500
#define CACHE_OVERTIME    1000   //10msΪ��λ, 10��֮ǰ�ļ�¼һ�ɺ���

void drv_RecordCache_init(void);
TAGCACHE* drv_RecordCache_pop(void);
TAGCACHE* drv_RecordCache_get(UInt32 pos);
bool fns_RecordCache_getByIndex(int index,TAGCACHE* tagCache);
int drv_RecordCache_push(TAGCACHE* tagCache);
TAGCACHE* drv_RecordCache_find(UInt16 coinId,UInt32 tagId);


UInt32 drv_RecordCache_sof(void);
UInt32 drv_RecordCache_eof(void);
UInt32 drv_RecordCache_count(void);
bool drv_RecordCache_isFull(void);
#endif
