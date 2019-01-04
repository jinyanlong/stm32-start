#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

#define FLASH_PAGEMASK (0xFFFFFC00)
#define FLASH_DEVICEID (0x1FFFF7E8+8)
#define DEVICEID (*(u32*)FLASH_DEVICEID)

#define FLASH_UID (0x1FFFF7E8)

#define DEV_TYPE  0x3046
#ifndef __DEBUG
    #ifdef PIGEON_V6
        #define DEV_VER   0x0689
    #else
        #define DEV_VER   0x0789
    #endif
    //   ,准备调整Exception...
    //X.89  SMS状态下也启用GPRS状态传输,优化网络传输,修改注册机制
    //X.88  扩大Task栈空间
    //X.87  修复不断重复刷屏,导致系统速度变慢的问题,修正HexToBin 最后没有置0的问题
    //X.86  ERROR:1201的错误，正在查找原因...,1. 修正存储布局错误,2,cacheFile读/写缓冲区冲突的BUG
    //X.85 去掉UID版本限制
    //X.84 支持FDXN,修复一些BUG
    //X.83 修正V7版足环测试的BUG,调整PING分时工作参数
    //X.82 修改TAG缓存机制,修复UID板不工作的问题, 台湾认发现文件系统会崩溃，需要继续检查
    //X.81 修正时钟精度到1ms
    //X.80 增加通讯超时,删除了出错保护时间
    //X.79 增加频段显示功能
    //X.77 修正与V6感应版匹配的一些问题
    //6.76 代码结构大幅度修改,增加公棚感应版,V7鸽钟版本
    //6.75 在sendRecord3中增加时区信息
    //6.74 GPS-$GPRMC返回的时间,直接就是UTC时间,不需要调整,改回
    //6.73 SMD: sendRecord->sendRecord3, dispalyNo不是正好6个字符时,SMS发送时长度计算有误
    //6.70 时区和UTC时间的修正
    //6.69 GPRS有效,但SMD发送连续失败多次,重启整个系统
    //6.68 发现一个严重的BUG,AT45DB161的programBuff实现有误,修改后可支持AT45DB161E
    //6.67 变更6.66的方案,已经通讯设置,决定是否显示用户短信菜单
    //6.66 修改了一个模式,当正式比赛启动时,不自动关闭用户SMS通知
    //6.64 去除设备有效期,改善3G启动时间
    //6.62
    //6.59 gps 凌晨4点复位
    //6.58 reset序列之后延时5秒
    //6.56 reset序列之后延时1秒
    //6.54 防止3G模块死机（监测at+creg异常则重启），修改模块开机序列，原来的不对
    //6.52 3G短信修复,区分3G模块版本和2G模块版本，时间校准的偏移量不同，3G模块开机防止自动关机
    //6.51gsm重启控制，错误次数达到3次才重启
    //6.49加入了gsm重启
    //6.48加入了gsm注册后用gsm注册时间校准系统时间
    //6.47修复多个鸽环毫秒不变化问题，gps长时间无反应复位
#else
    #define DEV_VER 0x0000
#endif

#define S_FORMAT_DATA_TAG "FOFIAQYH"
#define S_FORMAT_FONT_TAG "FOFIAFNT"
#define S_FORMAT_ALL_TAG  "FOFIAALL"

#define FOFIAKEY	"FOFIAKEY"

#define COMM_ADDR   0x8001

//////////////GUI TASK中的消息和事件,统一编码/////////////////////////
#define EVENT_GUI_TICK          0x01     //时间片
#define EVENT_GUI_MSG           0x02
#define MSGID_GUI_TESTSMS       0x08
#define MSGID_GUI_CHAR          0x10
#define MSGID_GUI_TAG           0x20
#define EVENT_GUI_COMM          0x40     //有通讯
#define EVENT_GUI_EXCEPTION     0x80     //异常



//////////////RACE TASK中的消息///////////////////////////////
#define MSGID_RACE_START        (1<<0)
#define MSGID_RACE_STOP         (1<<1)
#define MSGID_RACE_TIMETICK     (1<<2)
#define MSGID_RACE_ACTIONACK    (1<<3)
#define MSGID_RACE_TESTSMS      (1<<4)
#define MSGID_RACE_GPSTIME      (1<<5)
#define MSGID_RACE_REGTIME      (1<<6)
#define MSGID_RACE_GSMTIME      (1<<7)
#define MSGID_RACE_SERVERTIME   (1<<8)
//#define MSGID_RACE_LOCATION    (1<<9)    //从GPRS模块处获得的定位(改用EVENT)


/////////////////GSM TASK中的消息///////////////////////
#define MSGID_GSM_STARTACTION	       (1<<0)
#define MSGID_GSM_REQUESTRESULT	       (1<<1)
#define MSGID_GSM_CLOSEACTION	       (1<<2)
#define MSGID_GSM_SENDHTTP		       (1<<3)
#define MSGID_GSM_SENDSMS_TXT          (1<<4)
#define MSGID_GSM_SENDSMS_PDU16	       (1<<5)
#define MSGID_GSM_SENDHTTP_YZ	       (1<<6)
#define MSGID_GSM_LOCATION	           (1<<7)



//////////////////事件,所有任务都放在一起//////////////////////
#define EVENT_RACE_TICK         0x01     //时间片
#define EVENT_RACE_MSG          0x02     //消息队列中有数据
#define EVENT_RACE_RX_COMM      0x04     //通讯串口收到数据包
#define EVENT_RACE_RX_GPS       0x08     //GPS串口收到数据包
#define EVENT_RACE_TAGLIST      0x10     //发现标签队列有数据
#define EVENT_RACE_REUPLOAD     0x20     //重新上传记录


#define EVENT_GSM_TICK      0x01     //时间片
#define EVENT_GSM_MSG       0x02     //消息队列中有数据
#define EVENT_GSM_CHECK     0x04     //GSM任务中使用,查询状态
#define EVENT_GSM_RESET     0x08     //复位GSM模块



#define EVENT_CAN_TICK  0x01
#define EVENT_CAN_MSG   0x02


#define EVENT_TASK_ALL     0xFFFFFF





//模拟中断
#define EVENT_ISR_RX_GPS    EVENT_RACE_RX_GPS
#define EVENT_ISR_RX_COMM   EVENT_RACE_RX_COMM


#define TIME_MIN           86400000   //起始时刻(特意设置成从非0值开始)

//锁住操作FLASH的TASK, 此处直接锁OS
#define FLASH_LOCK   OS_LOCK
#define FLASH_UNLOCK OS_UNLOCK

#define RACETASK_LOCK    FLASH_LOCK
#define RACETASK_UNLOCK  FLASH_UNLOCK

//任务堆栈
#define STACKSIZE_GPRSTASK   200
#define STACKSIZE_RFTASK     200
#define STACKSIZE_CANTASK    200
#define STACKSIZE_GUITASK    400
#define STACKSIZE_RACETASK   400


#define TASK_GSM_OVERTIME    60000      //GSM模块,4G启动时间就需要10秒
#define TASK_RF_OVERTIME     3000
#define TASK_CAN_OVERTIME    3000
#define TASK_GUI_OVERTIME    3000
#define TASK_RACE_OVERTIME   3000


//任务优先级
//#define TASK_PRIORITY_TICK      tskIDLE_PRIORITY+6    //优先级最高,任何操作都不需要LOCK OS
#define TASK_PRIORITY_CAN       tskIDLE_PRIORITY+5      //要求处理速度比较高
#define TASK_PRIORITY_RACE      tskIDLE_PRIORITY+4      //工作例程(FLASH在此例程中直接操作)

#define TASK_PRIORITY_RF        tskIDLE_PRIORITY+3      //RF和GSM对处理速度要求不高
#define TASK_PRIORITY_GSM       tskIDLE_PRIORITY+1      //GSM模块失效时可能引起死循环(CTS/RTS导致),所以把优先级降到最低
#define TASK_PRIORITY_GUI       tskIDLE_PRIORITY+1


//中断优先级
#define ISR_PRIORITY_GROUP          NVIC_PriorityGroup_2
#define ISR_PRIORITY(Group,Sub)     (((Group)<<2)|(Sub))

#define ISR_PRIORITY_OS_HIGHEST      ISR_PRIORITY(2,0)

#define ISR_PRIORITY_UART            ISR_PRIORITY(0,0)   //STM32串口没有缓冲区，必须高于OS
#define ISR_PRIORITY_RF              ISR_PRIORITY(1,0)   //STM32串口没有缓冲区，必须高于OS
#define ISR_PRIORITY_CAN             ISR_PRIORITY_OS_HIGHEST



__INLINE void delay_ms(UInt32 _1ms){
    vTaskDelay((portTickType)_1ms);
}


#endif
