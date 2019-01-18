#include "includes.h"
#include "RealTimer.h"

FUNCTION_TICK s_hookTick=NULL;
TimerHandle_t _tickHandler;

static volatile Int64 _time_base;//ʱ��ƫ������
static volatile UInt32 _nowTick;
static volatile UInt32 _lastVerifyTick;

//����CPU�ڲ�RTCʱ��,���ȵ�1ms ,��DS1302/D3231����Ϊһ��ʱ��У׼Դ
void drv_Time_init(void){
    Int64 clockTime=0;
    _time_base=0;

    
    //����RTC��ʼֵ(RTC ��Զ����������Ҳ�������޸�,ֻ��ͨ��_time_base��������׼)
    hal_RTC_init(TIME_MIN);
    _lastVerifyTick=_nowTick=hal_RTC_getCount(); //ʱ�Ӵ�86400000��ʼ����, ��ƫ��һ��,��ֹ���ָ���
    
    if(utcToSeconds(clockTime,UTC_S_2000)>10*365*86400){
        _time_base=clockTime-_nowTick;//����ʱ�ӻ�׼
    }else{
        _time_base=0;
    }
            
    //�����������
    srand((int)(_time_base/1000));
}


bool drv_Time_isValid(void){
    return _time_base!=0;
}

UInt32 isr_Time_getTick(void){
    return _nowTick;
}

UInt32 drv_Time_getTick(void){
    return _nowTick;
}

Int64 drv_Time_get(void){
    return _time_base+_nowTick;
}

//static void drv_Time_load_(DATETIME* clockTime){
//    Int64 clock_now;
//    Int64 rtc_now;
//    
//    clock_now=parseTime(clockTime,0)+500; //ת��ʱ������500ms,ƽ�����ֵ
//    rtc_now=drv_Time_get();
//    
//    //�����뼶����,ִ������
//    if( (clock_now>(rtc_now+999)) || (rtc_now>(999+clock_now)) ){
//        _time_base=clock_now-_nowTick;   
//    }
//}


void drv_Time_load(void){
#ifdef PIGEON_V7
    DATETIME clockTime;
    bool needVerify=false;
    
    taskDISABLE_INTERRUPTS();
    //ÿ5��˶�һ��
    if ( (_nowTick-_lastVerifyTick)>5000 ){
        needVerify=true;
        _lastVerifyTick=_nowTick+5000;
    }
    taskENABLE_INTERRUPTS();
    
    if(needVerify){
        if(!hal_DS3231_getTime(&clockTime)) return;  //�쳣,����
        
        taskDISABLE_INTERRUPTS();
        drv_Time_load_(&clockTime);
        taskENABLE_INTERRUPTS();
    }
#endif
}


void drv_Time_set(Int64 masterTime){
    //DS1302�����Ƚ��������Բ�����OS,��һ��Ҫȷ����дֻ��ͬһ��������ִ��
#ifdef    PIGEON_V7
    { //д�뵽DS1302(UTC),
        DATETIME dt;
        printTime(masterTime,0,&dt);
        hal_DS3231_setTime(&dt);

    }
#endif
    taskDISABLE_INTERRUPTS();
    _nowTick=hal_RTC_getCount();
    _time_base=masterTime-_nowTick;
    taskENABLE_INTERRUPTS();
}


//�����ʱ�侫��Ҫ��ߣ��͵������������ֱ�Ӷ�ȡӲ��ʱ��
Int64 fns_Time_flush(void){
    Int64 val;
    taskDISABLE_INTERRUPTS();
    _nowTick=hal_RTC_getCount();
    val=drv_Time_get();
    taskENABLE_INTERRUPTS();
    return val;
}

Int64 fns_Time_get(void){
    Int64 val;
    taskDISABLE_INTERRUPTS();
    val=drv_Time_get();
    taskENABLE_INTERRUPTS();
    return val;
}



extern void drv_System_tick(UInt32 nowTick);
//������ߵ��������ȼ�,����һ�ɲ���ҪtaskDISABLE_INTERRUPTS/taskENABLE_INTERRUPTS
void drv_TimeTick_callback( TimerHandle_t xTimer ){//�����ʱ��tick
    _nowTick=hal_RTC_getCount();//������ˢ��ʱ��
    if(_nowTick > 0xFFFF0000){
        NVIC_SystemReset();  //��������40��Ż�ִ�е�����,�����ڷ���ϵͳBUG
    }

    drv_System_tick(_nowTick);
}


void drv_TimeTick_init(void){
    static StaticTimer_t _timer;
    
    _tickHandler = xTimerCreateStatic("TICK",SYSTICK_MS,pdTRUE,( void * ) 0,drv_TimeTick_callback,&_timer);
    if( _tickHandler != NULL ) {
        xTimerStart(_tickHandler, 0 );
    }
}

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize ){
	static StaticTask_t _task_tcb;
    static StackType_t _task_stack[configTIMER_TASK_STACK_DEPTH];
    
    *ppxTimerTaskTCBBuffer=&_task_tcb;
    *ppxTimerTaskStackBuffer=_task_stack;
    *pulTimerTaskStackSize=sizeof(_task_stack)/sizeof(StackType_t);
}

