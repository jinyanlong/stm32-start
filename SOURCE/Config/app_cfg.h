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
    //   ,׼������Exception...
    //X.89  SMS״̬��Ҳ����GPRS״̬����,�Ż����紫��,�޸�ע�����
    //X.88  ����Taskջ�ռ�
    //X.87  �޸������ظ�ˢ��,����ϵͳ�ٶȱ���������,����HexToBin ���û����0������
    //X.86  ERROR:1201�Ĵ������ڲ���ԭ��...,1. �����洢���ִ���,2,cacheFile��/д��������ͻ��BUG
    //X.85 ȥ��UID�汾����
    //X.84 ֧��FDXN,�޸�һЩBUG
    //X.83 ����V7���㻷���Ե�BUG,����PING��ʱ��������
    //X.82 �޸�TAG�������,�޸�UID�岻����������, ̨���Ϸ����ļ�ϵͳ���������Ҫ�������
    //X.81 ����ʱ�Ӿ��ȵ�1ms
    //X.80 ����ͨѶ��ʱ,ɾ���˳�����ʱ��
    //X.79 ����Ƶ����ʾ����
    //X.77 ������V6��Ӧ��ƥ���һЩ����
    //6.76 ����ṹ������޸�,���ӹ����Ӧ��,V7���Ӱ汾
    //6.75 ��sendRecord3������ʱ����Ϣ
    //6.74 GPS-$GPRMC���ص�ʱ��,ֱ�Ӿ���UTCʱ��,����Ҫ����,�Ļ�
    //6.73 SMD: sendRecord->sendRecord3, dispalyNo��������6���ַ�ʱ,SMS����ʱ���ȼ�������
    //6.70 ʱ����UTCʱ�������
    //6.69 GPRS��Ч,��SMD��������ʧ�ܶ��,��������ϵͳ
    //6.68 ����һ�����ص�BUG,AT45DB161��programBuffʵ������,�޸ĺ��֧��AT45DB161E
    //6.67 ���6.66�ķ���,�Ѿ�ͨѶ����,�����Ƿ���ʾ�û����Ų˵�
    //6.66 �޸���һ��ģʽ,����ʽ��������ʱ,���Զ��ر��û�SMS֪ͨ
    //6.64 ȥ���豸��Ч��,����3G����ʱ��
    //6.62
    //6.59 gps �賿4�㸴λ
    //6.58 reset����֮����ʱ5��
    //6.56 reset����֮����ʱ1��
    //6.54 ��ֹ3Gģ�����������at+creg�쳣�����������޸�ģ�鿪�����У�ԭ���Ĳ���
    //6.52 3G�����޸�,����3Gģ��汾��2Gģ��汾��ʱ��У׼��ƫ������ͬ��3Gģ�鿪����ֹ�Զ��ػ�
    //6.51gsm�������ƣ���������ﵽ3�β�����
    //6.49������gsm����
    //6.48������gsmע�����gsmע��ʱ��У׼ϵͳʱ��
    //6.47�޸�����뻷���벻�仯���⣬gps��ʱ���޷�Ӧ��λ
#else
    #define DEV_VER 0x0000
#endif

#define S_FORMAT_DATA_TAG "FOFIAQYH"
#define S_FORMAT_FONT_TAG "FOFIAFNT"
#define S_FORMAT_ALL_TAG  "FOFIAALL"

#define FOFIAKEY	"FOFIAKEY"

#define COMM_ADDR   0x8001

//////////////GUI TASK�е���Ϣ���¼�,ͳһ����/////////////////////////
#define EVENT_GUI_TICK          0x01     //ʱ��Ƭ
#define EVENT_GUI_MSG           0x02
#define MSGID_GUI_TESTSMS       0x08
#define MSGID_GUI_CHAR          0x10
#define MSGID_GUI_TAG           0x20
#define EVENT_GUI_COMM          0x40     //��ͨѶ
#define EVENT_GUI_EXCEPTION     0x80     //�쳣



//////////////RACE TASK�е���Ϣ///////////////////////////////
#define MSGID_RACE_START        (1<<0)
#define MSGID_RACE_STOP         (1<<1)
#define MSGID_RACE_TIMETICK     (1<<2)
#define MSGID_RACE_ACTIONACK    (1<<3)
#define MSGID_RACE_TESTSMS      (1<<4)
#define MSGID_RACE_GPSTIME      (1<<5)
#define MSGID_RACE_REGTIME      (1<<6)
#define MSGID_RACE_GSMTIME      (1<<7)
#define MSGID_RACE_SERVERTIME   (1<<8)
//#define MSGID_RACE_LOCATION    (1<<9)    //��GPRSģ�鴦��õĶ�λ(����EVENT)


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
#define EVENT_RACE_TICK         0x01     //ʱ��Ƭ
#define EVENT_RACE_MSG          0x02     //��Ϣ������������
#define EVENT_RACE_RX_COMM      0x04     //ͨѶ�����յ����ݰ�
#define EVENT_RACE_RX_GPS       0x08     //GPS�����յ����ݰ�
#define EVENT_RACE_TAGLIST      0x10     //���ֱ�ǩ����������
#define EVENT_RACE_REUPLOAD     0x20     //�����ϴ���¼


#define EVENT_GSM_TICK      0x01     //ʱ��Ƭ
#define EVENT_GSM_MSG       0x02     //��Ϣ������������
#define EVENT_GSM_CHECK     0x04     //GSM������ʹ��,��ѯ״̬
#define EVENT_GSM_RESET     0x08     //��λGSMģ��



#define EVENT_CAN_TICK  0x01
#define EVENT_CAN_MSG   0x02


#define EVENT_TASK_ALL     0xFFFFFF





//ģ���ж�
#define EVENT_ISR_RX_GPS    EVENT_RACE_RX_GPS
#define EVENT_ISR_RX_COMM   EVENT_RACE_RX_COMM


#define TIME_MIN           86400000   //��ʼʱ��(�������óɴӷ�0ֵ��ʼ)

//��ס����FLASH��TASK, �˴�ֱ����OS
#define FLASH_LOCK   OS_LOCK
#define FLASH_UNLOCK OS_UNLOCK

#define RACETASK_LOCK    FLASH_LOCK
#define RACETASK_UNLOCK  FLASH_UNLOCK

//�����ջ
#define STACKSIZE_GPRSTASK   200
#define STACKSIZE_RFTASK     200
#define STACKSIZE_CANTASK    200
#define STACKSIZE_GUITASK    400
#define STACKSIZE_RACETASK   400


#define TASK_GSM_OVERTIME    60000      //GSMģ��,4G����ʱ�����Ҫ10��
#define TASK_RF_OVERTIME     3000
#define TASK_CAN_OVERTIME    3000
#define TASK_GUI_OVERTIME    3000
#define TASK_RACE_OVERTIME   3000


//�������ȼ�
//#define TASK_PRIORITY_TICK      tskIDLE_PRIORITY+6    //���ȼ����,�κβ���������ҪLOCK OS
#define TASK_PRIORITY_CAN       tskIDLE_PRIORITY+5      //Ҫ�����ٶȱȽϸ�
#define TASK_PRIORITY_RACE      tskIDLE_PRIORITY+4      //��������(FLASH�ڴ�������ֱ�Ӳ���)

#define TASK_PRIORITY_RF        tskIDLE_PRIORITY+3      //RF��GSM�Դ����ٶ�Ҫ�󲻸�
#define TASK_PRIORITY_GSM       tskIDLE_PRIORITY+1      //GSMģ��ʧЧʱ����������ѭ��(CTS/RTS����),���԰����ȼ��������
#define TASK_PRIORITY_GUI       tskIDLE_PRIORITY+1


//�ж����ȼ�
#define ISR_PRIORITY_GROUP          NVIC_PriorityGroup_2
#define ISR_PRIORITY(Group,Sub)     (((Group)<<2)|(Sub))

#define ISR_PRIORITY_OS_HIGHEST      ISR_PRIORITY(2,0)

#define ISR_PRIORITY_UART            ISR_PRIORITY(0,0)   //STM32����û�л��������������OS
#define ISR_PRIORITY_RF              ISR_PRIORITY(1,0)   //STM32����û�л��������������OS
#define ISR_PRIORITY_CAN             ISR_PRIORITY_OS_HIGHEST



__INLINE void delay_ms(UInt32 _1ms){
    vTaskDelay((portTickType)_1ms);
}


#endif
