#ifndef __RECORDLIST_H
#define __RECORDLIST_H

#include "SFTypes.h"

#include "RaceData.h"

#define COIN_STATE_MASK                     0x03
    #define COIN_STATE_ONLINE               0x00        //在线(已保存记录)
    #define COIN_STATE_OFFLINE              0x01        //离线(已保存记录)
    #define COIN_STATE_OFFLINING            0x02        //离线(尚未保存记录)
#define TAG_FLAG_MASK                       0x80
    #define TAG_FLAG_LOWPOWER               0x80        //低电压报警，在flag的最高位

//由于通讯延迟的存在,采集器时间与标签时间并不能互操作,所以需要用标签Tick做校准
//这里的Tick都是以标签Tick为基准, 真实的时间=(baseTime+tick*10)
typedef struct _TAGCACHE{
    Int64 baseTime;       //标签0时刻对应的UTC时间(从全局范围来看并不是很精确，但"同一轮进入"是准确的)
    
    UInt8 state;            //状态 最高位1为低电压，后2位表示在线离线
    UInt16 coinId;          //coinId带ONLINE标志

    UInt32 tagId;    
    UInt32 startTick;   //开始时间(秒),主要用途是与activeTime一起校准时间
    UInt32 activeTick;  //激活时刻(10ms为单位)
    UInt32 offTick;     //离开时间(10ms为单位)
}TAGCACHE;


//typedef struct _TAGCACHE{    
//    UInt8 state;            //状态 最高位1为低电压，后2位表示在线离线
//    UInt8 startTime_10ms;
//    UInt8 activeTime_10ms;
//    UInt8 offTime_10ms;
//    
//    UInt32 recordTime;  //最后一次记录在线保存时刻
//    
//    UInt32 startTime;   //开始时间(秒),主要用途是与startBase一起校准时间
//    UInt32 activeTime;  //激活时间(秒)
//    UInt32 offTime;     //离开时间(秒)

//    UInt32 tagId;
//    UInt16 coinId;          //coinId带ONLINE标志
//}TAGCACHE;

#define CACHE_CAPACITY     500
#define CACHE_OVERTIME    1000   //10ms为单位, 10秒之前的记录一律忽略

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
