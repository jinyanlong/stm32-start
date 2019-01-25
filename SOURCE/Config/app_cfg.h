#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

#ifdef STM32F10X_LD
    #error "STM32F10X_LD is unsupport!"
#elif defined STM32F10X_MD
#elif defined STM32F10X_HD
#elif defined STM32F10X_CL
#else
    #error "Please select first the STM32 device to be used (in stm32f10x.h)"    
#endif

#define ROM_ADDRESS_START 0x08000000
#define ROM_PAGESIZE      1024                   //1K
#define ROM_CONFIG_PAGE   120                    //尾部留8K字节,写配置信息

#define WORKMODE_FLAG_NET    0x01       //网络上传使能
#define WORKMODE_FLAG_UART   0x02       //串口上传使能


//#define FLASH_PAGEMASK (0xFFFFFC00)
#define ROM_UID_OFFSET (0x1FFFF7E8)
#define DEVICEID (*(u32*)(ROM_UID_OFFSET+8))


#define DEV_TYPE  0x3014    //新版激活器
#define DEV_VER   0x0001


#define S_FORMAT_DATA_TAG "FOFIAXYK"
#define S_FORMAT_FONT_TAG "FOFIAFNT"
#define S_FORMAT_ALL_TAG  "FOFIAALL"

#define FOFIAKEY	"FOFIAKEY"

//////////////GUI TASK中的消息和事件,统一编码/////////////////////////
#define EVENT_GUI_TICK          0x01     //时间片
#define EVENT_GUI_MSG           0x02
#define MSGID_GUI_CHAR          0x10
//#define MSGID_GUI_TAG           0x20
#define EVENT_GUI_COMM          0x40     //有通讯
#define EVENT_GUI_EXCEPTION     0x80     //异常



//////////////RACE TASK中的消息///////////////////////////////
#define MSGID_RACE_START        (1<<0)
#define MSGID_RACE_STOP         (1<<1)
#define MSGID_RACE_TIMETICK     (1<<2)
#define MSGID_RACE_ACTIONACK    (1<<3)
#define MSGID_RACE_GPSTIME      (1<<5)
#define MSGID_RACE_GSMTIME      (1<<7)

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
#define EVENT_MAIN_TICK         0x01     //时间片
#define EVENT_MAIN_MSG          0x02     //消息队列中有数据
#define EVENT_MAIN_RX_COMM      0x04     //通讯串口收到数据包
#define EVENT_MAIN_RX_COMM485      0x08    


#define EVENT_RX_NRFCOMM     0x01     //2.G串口数据
#define EVENT_RX_PCCOMM     0x02     //对外pc串口


#define EVENT_CAN_TICK  0x01
#define EVENT_CAN_MSG   0x02


#define EVENT_TASK_ALL     0x00FFFFFF


//模拟中断
#define EVENT_ISR_RX_PCCOMM   EVENT_RX_PCCOMM //pc通信口
#define EVENT_ISR_RX_NRFCOMM   EVENT_RX_NRFCOMM //pc通信口

#define TIME_MIN           86400000   //起始时刻(特意设置成从非0值开始)

//锁住操作FLASH的TASK, 此处直接锁OS
#define FLASH_LOCK   OS_LOCK
#define FLASH_UNLOCK OS_UNLOCK

#define RACETASK_LOCK    FLASH_LOCK
#define RACETASK_UNLOCK  FLASH_UNLOCK

//任务堆栈
#define STACKSIZE_GPRSTASK   200
#define STACKSIZE_GUITASK    400
#define STACKSIZE_MAINTASK   400


#define TASK_GSM_OVERTIME    60000      //GSM模块,4G启动时间就需要10秒
#define TASK_RF_OVERTIME     3000
#define TASK_CAN_OVERTIME    3000
#define TASK_GUI_OVERTIME    10000
#define TASK_RACE_OVERTIME   3000


//任务优先级
//#define TASK_PRIORITY_TICK      tskIDLE_PRIORITY+6    //优先级最高,任何操作都不需要LOCK OS
#define TASK_PRIORITY_CAN       tskIDLE_PRIORITY+5      //要求处理速度比较高
#define TASK_PRIORITY_MAIN      tskIDLE_PRIORITY+4      //工作例程(FLASH在此例程中直接操作)

#define TASK_PRIORITY_RF        tskIDLE_PRIORITY+3      //RF和GSM对处理速度要求不高
#define TASK_PRIORITY_GSM       tskIDLE_PRIORITY+2      //GSM模块失效时可能引起死循环(CTS/RTS导致),所以把优先级降到最低
#define TASK_PRIORITY_GUI       tskIDLE_PRIORITY+1


//中断优先级
#define ISR_PRIORITY_GROUP          NVIC_PriorityGroup_2
#define ISR_PRIORITY(Group,Sub)     (((Group)<<2)|(Sub))

#define ISR_PRIORITY_OS_HIGHEST      ISR_PRIORITY(2,0)

#define ISR_PRIORITY_UART            ISR_PRIORITY(0,0)   //STM32串口没有缓冲区，必须高于OS
#define ISR_PRIORITY_RF              ISR_PRIORITY(1,0)   //STM32串口没有缓冲区，必须高于OS
#define ISR_PRIORITY_CAN             ISR_PRIORITY_OS_HIGHEST

// __INLINE void delay_ms(UInt32 _1ms){
//     vTaskDelay((portTickType)_1ms);
// }

#define __LED 1


#endif
