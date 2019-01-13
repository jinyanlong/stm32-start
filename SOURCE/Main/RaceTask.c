#include "includes.h"
#include "halCPU.h"
#include "tagList.h"
#include "RaceTask.h"
#include "RaceFunc.h"
#include "SFile.h"
#include "FileFunc.h"
#include "RaceFunc.h"
//#include "GPS.h"
#include "CommDriver.h"
#include "RealTimer.h"
//#include "Beep.h"
#include "SMD.h"
#include "mem_alloc.h"
//#include "gprs.h"
//#include "tagReceiver.h"
#include "RealTimer.h"
#include "Func_school.h"
#include "MainFunc.h"
#include "Func_Common.h"
#include "PcComm.h"
#include "Nrf.h"
#include "halNrf.h"

#ifdef __NET
    //Ϊ��ʡ�ڴ�,HTTP���շ�����������s_http
    extern HTTPSTRUCT s_http;
#endif

static UInt32 _raceTask_stack[STACKSIZE_RACETASK];
static StaticTask_t _raceTask_tcb;
static StaticEventGroup_t m_raceEvents_pcb;
static EventGroupHandle_t m_raceEvents;
static xQueueHandle m_raceQueue;
volatile UInt32 s_race_activeTick;
extern volatile UInt32 s_gui_activeTick;
extern volatile UInt32 s_gprs_activeTick;

extern NrfUartPacketHandler _nrfCommUart;
extern UartPacketHandler _pcCommUart;


UInt8 ROM_WORKMODE;
UInt8 ROM_PROXYID[2];


UInt8 s_systemId[4]={0,0,0,0};  //ǰ2�ֽ�Ϊ�����̱��,��2�ֽ�ΪѧУ���
UInt32 s_systemMark=0;

UInt8 s_systemKey[8];
UInt8 _systemRnd[8];
DESKEYSTRUCT desKey; //desKeyʵ�����Ǿֲ�����,������ȫ��,��Ϊ�˼���ջ�ռ�����

volatile MONITOR s_monitor;
bool m_RaceTask_ready=false;
void drv_RaceTask_init(void){
    static StaticQueue_t _queue_pcb;
    static UInt8 _queue_buffer[50*sizeof(MESSAGE)];
    
    ROM_WORKMODE=~(*(u8*)(ROM_ADDRESS_START+ROM_PAGESIZE*ROM_CONFIG_PAGE) );
    ROM_PROXYID[0]=~(*(u8*)(ROM_ADDRESS_START+ROM_PAGESIZE*ROM_CONFIG_PAGE+2) );
    ROM_PROXYID[1]=~(*(u8*)(ROM_ADDRESS_START+ROM_PAGESIZE*ROM_CONFIG_PAGE+3) );

    memset(s_systemId,0,4);
    s_systemMark=0;

    s_race_activeTick=drv_Time_getTick();

    //������Ϣ����
    m_raceQueue = xQueueCreateStatic(sizeof(_queue_buffer)/sizeof(MESSAGE), sizeof(MESSAGE), _queue_buffer,&_queue_pcb );
    
    m_raceEvents=xEventGroupCreateStatic(&m_raceEvents_pcb);   
    xTaskCreateStatic(drv_RaceTask_run, "RACE", STACKSIZE_RACETASK, NULL,TASK_PRIORITY_RACE, _raceTask_stack, &_raceTask_tcb );    
}

void event_RaceTask_raise(UInt32 event){
    xEventGroupSetBits(m_raceEvents,event);
}
void isr_RaceTask_notify(UInt32 event){
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    xEventGroupSetBitsFromISR(m_raceEvents,event,&xHigherPriorityTaskWoken);

    if( xHigherPriorityTaskWoken){
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
}

////////////////////��Ϣϵͳ���ڴ����///////////////////////////////////
//��: ����Ϊ�ڴ��ܹ���ʱ�ͷ�,���������_msgBuffer
#define  RACE_MESSAGE_BUFFER_SIZE (2048-8)
static UInt32 _raceMsgBuffer[RACE_MESSAGE_BUFFER_SIZE/4+2]={0,RACE_MESSAGE_BUFFER_SIZE};
void* drv_RaceMemory_alloc(UInt32 nSize){
    return fns_MessageMemory_alloc(&_raceMsgBuffer,nSize);
}
void drv_RaceMemory_free(void* p){
    fns_MessageMemory_free(p);
}

void msg_RaceTask_post(MESSAGE* msg){
    xQueueSend(m_raceQueue,msg,0);
    
    event_RaceTask_raise(EVENT_RACE_MSG);//׷��һ����Ϣ��־λ
}

bool drv_RaceTask_isReady(void){
    return m_RaceTask_ready;
}


void drv_RaceTask_doTick(UInt32 nowTick){
    if(_pcCommUart.lazyMode){
        if(nowTick>_pcCommUart.rspTimout){
            UInt8 protocol=0x80; //�޵�ַģʽ
            drv_CommFunc_setError(&_pcCommUart.rspPacket,ERRCODE_CMD_OVERTIME);         
            drv_Comm_sendPacket(_pcCommUart.pUart,&protocol,&_pcCommUart.rspPacket);
            drv_Comm_startRx(&_pcCommUart);
        }
    }
}


void drv_RaceTask_onTick(void){
    UInt32 nowTick;

    s_race_activeTick=nowTick=drv_Time_getTick();
   
#ifdef __GUI    
    event_GuiTask_raise(EVENT_GUI_TICK);
#endif    

    if( drv_Exception_get()!=NULL) return; 
 
    drv_RaceTask_doTick(nowTick);
}

void drv_RaceTask_load(void){
    memset((void*)&s_monitor,0,sizeof(s_monitor));
    
    if(ROM_WORKMODE==0){
        //ϵͳ��δ��ʽ��
        // drv_Exception_throw(ERROR_FILE_FORMAT);
    }
    
    //��������
    drv_Data_getSystem(s_systemId,&s_systemMark,s_systemKey);
    if( (ROM_PROXYID[0]!=s_systemId[0]) || (ROM_PROXYID[1]!=s_systemId[1]) ){
        //�����̱��벻ƥ��
        // drv_Exception_throw(ERROR_FILE_SYSID);
    }
        
    //��ȡ��ʼ����
	s_monitor.timezone=drv_Data_getTimeZone();
    s_monitor.totalSOF=drv_Data_getTotalSOF();
    s_monitor.totalEOF=drv_Data_getTotalEOF();
    s_monitor.uploadSOF=drv_Data_getUploadSOF();
    s_monitor.uartUploadSOF=drv_Data_getUartUploadSOF();
    s_monitor.uartOvertime=drv_Data_getUartOvertime();
    s_monitor.uartPingInterval=drv_Data_getUartPingInterval();
    s_monitor.tagOfftime=drv_Data_getTagOfftime();
    s_monitor.tagPingInterval=drv_Data_getTagPingInterval();
    s_monitor.webPingInterval=drv_Data_getWebPingInterval();
    s_monitor.todaySOF=s_monitor.totalEOF;

}

void drv_RaceTask_onMsg(MESSAGE* pMsg){
    switch(pMsg->nId){
#ifdef __NET
    case MSGID_RACE_GSMTIME:
        drv_RaceTask_onVerifyTime(pMsg->nId,pMsg->p1);
        break;
    case MSGID_RACE_ACTIONACK:
        drv_RaceTask_onActionACK(pMsg);
        break;
#endif
    default:
        break;
    }
}



void drv_RaceTask_doEvents(UInt32 eventMask){
    EventBits_t event;
    
    s_race_activeTick=drv_Time_getTick();
    m_RaceTask_ready=true;
    drv_nrfComm_startRx(&_nrfCommUart);  
    drv_Comm_startRx(&_pcCommUart);  

    
    while(1){
        event= xEventGroupWaitBits(m_raceEvents,EVENT_TASK_ALL,pdTRUE,pdFALSE,portMAX_DELAY);

        if(event&EVENT_RX_PCCOMM){            
            drv_PcComm_onEvent();
            // drv_nrfComm_startRx(&_nrfCommUart);
            #ifdef __GUI             
                event_GuiTask_raise(EVENT_GUI_COMM);//֪ͨ�������
            #endif            
        }
        
        if(event&EVENT_RX_NRFCOMM){            
            drv_NrfComm_onEvent();
            // drv_Comm_startRx(&_pcCommUart);
            #ifdef __GUI             
                event_GuiTask_raise(EVENT_GUI_COMM);//֪ͨ�������
            #endif            
        }

        if(event&EVENT_RACE_TICK){
            drv_RaceTask_onTick();
        }
                
        if( drv_Exception_get()!=0) continue;//ϵͳ�쳣,����ͨѶ,������������ֹ
        
        if(event&EVENT_RACE_MSG){
            //�����Ϣ����,�������ִ��
            MESSAGE msg;
    
            while( xQueueReceive(m_raceQueue,&msg,0)==pdTRUE ){//xQueuePeek(m_raceQueue,&msg,0)==pdTRUE
                drv_RaceTask_onMsg(&msg);
            }
        }
    }
}
bool drv_RaceTask_isCurrent(void){
    //��⵱ǰ�����Ƿ�RaceTask
    return (&_raceTask_tcb)==xTaskGetCurrentTaskHandle();
}

void drv_RaceTask_run(void* p){
    drv_RaceTask_load();      //��������,�������ݾ�������������

    drv_RaceTask_doEvents(EVENT_TASK_ALL);
}

