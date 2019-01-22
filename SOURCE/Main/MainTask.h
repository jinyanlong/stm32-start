#ifndef __RACETASK_001
#define __RACETASK_001


#define WORKMODE_VALID 0x80
#define WORKMODE_UART  0x01
#define WORKMODE_NET   0x02


//必须是非0值
#define ACTION_NONE              0
#define ACTION_SMD_PING 	     0x01
#define ACTION_SMD_UPLOADRECORD  0x02
#define ACTION_SMD_REGISTER      0x03

typedef struct _MONITOR_UPLOAD{
    UInt8  ins;
    UInt32 recordIndex;         //当前正在传输的记录索引
    UInt32 recordCount;         //一次传输记录的数量
}MONITOR_UPLOAD;


typedef struct _MONITOR{
    Int64  validate;
    UInt32 totalSOF;
    UInt32 totalEOF;
    UInt32 uploadSOF;
    UInt32 uartUploadSOF;
    UInt32 todaySOF;
    short  timezone;
    
    UInt16 tagPingInterval;   //标签PING间隔,默认1分钟
    UInt16 tagOfftime;        //标签离线的超时值,默认3秒,采集器暂时没有使用
    UInt16 webPingInterval;
    
    UInt16 uartPingInterval;
    UInt16 uartOvertime;
    
    UInt32 csqQueryTick;
    UInt32 pingTick;
    UInt32 netProtectTick;
    UInt32 nextLocationTick;
    
    //UInt32 state;   //工作状态
    
    MONITOR_UPLOAD upload;
    //GPSINFO gps;
}MONITOR;

bool drv_RaceTask_isReady(void);
void event_RaceTask_raise(UInt32 event);
void drv_MainTask_init(void);

#endif
