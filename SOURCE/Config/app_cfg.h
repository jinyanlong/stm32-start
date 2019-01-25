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
#define ROM_CONFIG_PAGE   120                    //β����8K�ֽ�,д������Ϣ

#define WORKMODE_FLAG_NET    0x01       //�����ϴ�ʹ��
#define WORKMODE_FLAG_UART   0x02       //�����ϴ�ʹ��


//#define FLASH_PAGEMASK (0xFFFFFC00)
#define ROM_UID_OFFSET (0x1FFFF7E8)
#define DEVICEID (*(u32*)(ROM_UID_OFFSET+8))


#define DEV_TYPE  0x3014    //�°漤����
#define DEV_VER   0x0001


#define S_FORMAT_DATA_TAG "FOFIAXYK"
#define S_FORMAT_FONT_TAG "FOFIAFNT"
#define S_FORMAT_ALL_TAG  "FOFIAALL"

#define FOFIAKEY	"FOFIAKEY"

//////////////GUI TASK�е���Ϣ���¼�,ͳһ����/////////////////////////
#define EVENT_GUI_TICK          0x01     //ʱ��Ƭ
#define EVENT_GUI_MSG           0x02
#define MSGID_GUI_CHAR          0x10
//#define MSGID_GUI_TAG           0x20
#define EVENT_GUI_COMM          0x40     //��ͨѶ
#define EVENT_GUI_EXCEPTION     0x80     //�쳣



//////////////RACE TASK�е���Ϣ///////////////////////////////
#define MSGID_RACE_START        (1<<0)
#define MSGID_RACE_STOP         (1<<1)
#define MSGID_RACE_TIMETICK     (1<<2)
#define MSGID_RACE_ACTIONACK    (1<<3)
#define MSGID_RACE_GPSTIME      (1<<5)
#define MSGID_RACE_GSMTIME      (1<<7)

/////////////////GSM TASK�е���Ϣ///////////////////////
#define MSGID_GSM_STARTACTION	       (1<<0)
#define MSGID_GSM_REQUESTRESULT	       (1<<1)
#define MSGID_GSM_CLOSEACTION	       (1<<2)
#define MSGID_GSM_SENDHTTP		       (1<<3)
#define MSGID_GSM_SENDSMS_TXT          (1<<4)
#define MSGID_GSM_SENDSMS_PDU16	       (1<<5)
#define MSGID_GSM_SENDHTTP_YZ	       (1<<6)
#define MSGID_GSM_LOCATION	           (1<<7)



//////////////////�¼�,�������񶼷���һ��//////////////////////
#define EVENT_MAIN_TICK         0x01     //ʱ��Ƭ
#define EVENT_MAIN_MSG          0x02     //��Ϣ������������
#define EVENT_MAIN_RX_COMM      0x04     //ͨѶ�����յ����ݰ�
#define EVENT_MAIN_RX_COMM485      0x08    


#define EVENT_RX_NRFCOMM     0x01     //2.G��������
#define EVENT_RX_PCCOMM     0x02     //����pc����


#define EVENT_CAN_TICK  0x01
#define EVENT_CAN_MSG   0x02


#define EVENT_TASK_ALL     0x00FFFFFF


//ģ���ж�
#define EVENT_ISR_RX_PCCOMM   EVENT_RX_PCCOMM //pcͨ�ſ�
#define EVENT_ISR_RX_NRFCOMM   EVENT_RX_NRFCOMM //pcͨ�ſ�

#define TIME_MIN           86400000   //��ʼʱ��(�������óɴӷ�0ֵ��ʼ)

//��ס����FLASH��TASK, �˴�ֱ����OS
#define FLASH_LOCK   OS_LOCK
#define FLASH_UNLOCK OS_UNLOCK

#define RACETASK_LOCK    FLASH_LOCK
#define RACETASK_UNLOCK  FLASH_UNLOCK

//�����ջ
#define STACKSIZE_GPRSTASK   200
#define STACKSIZE_GUITASK    400
#define STACKSIZE_MAINTASK   400


#define TASK_GSM_OVERTIME    60000      //GSMģ��,4G����ʱ�����Ҫ10��
#define TASK_RF_OVERTIME     3000
#define TASK_CAN_OVERTIME    3000
#define TASK_GUI_OVERTIME    10000
#define TASK_RACE_OVERTIME   3000


//�������ȼ�
//#define TASK_PRIORITY_TICK      tskIDLE_PRIORITY+6    //���ȼ����,�κβ���������ҪLOCK OS
#define TASK_PRIORITY_CAN       tskIDLE_PRIORITY+5      //Ҫ�����ٶȱȽϸ�
#define TASK_PRIORITY_MAIN      tskIDLE_PRIORITY+4      //��������(FLASH�ڴ�������ֱ�Ӳ���)

#define TASK_PRIORITY_RF        tskIDLE_PRIORITY+3      //RF��GSM�Դ����ٶ�Ҫ�󲻸�
#define TASK_PRIORITY_GSM       tskIDLE_PRIORITY+2      //GSMģ��ʧЧʱ����������ѭ��(CTS/RTS����),���԰����ȼ��������
#define TASK_PRIORITY_GUI       tskIDLE_PRIORITY+1


//�ж����ȼ�
#define ISR_PRIORITY_GROUP          NVIC_PriorityGroup_2
#define ISR_PRIORITY(Group,Sub)     (((Group)<<2)|(Sub))

#define ISR_PRIORITY_OS_HIGHEST      ISR_PRIORITY(2,0)

#define ISR_PRIORITY_UART            ISR_PRIORITY(0,0)   //STM32����û�л��������������OS
#define ISR_PRIORITY_RF              ISR_PRIORITY(1,0)   //STM32����û�л��������������OS
#define ISR_PRIORITY_CAN             ISR_PRIORITY_OS_HIGHEST

// __INLINE void delay_ms(UInt32 _1ms){
//     vTaskDelay((portTickType)_1ms);
// }

#define __LED 1


#endif
