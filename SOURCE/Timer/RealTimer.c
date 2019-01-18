#include "includes.h"
#include "RealTimer.h"

FUNCTION_TICK s_hookTick=NULL;
TimerHandle_t _tickHandler;

static volatile Int64 _time_base;//时间偏差秒数
static volatile UInt32 _nowTick;
static volatile UInt32 _lastVerifyTick;

//启用CPU内部RTC时钟,精度到1ms ,而DS1302/D3231仅作为一个时钟校准源
void drv_Time_init(void){
    Int64 clockTime=0;
    _time_base=0;

    
    //设置RTC初始值(RTC 永远递增不减，也不允许修改,只能通过_time_base来调整基准)
    hal_RTC_init(TIME_MIN);
    _lastVerifyTick=_nowTick=hal_RTC_getCount(); //时钟从86400000开始计数, 正偏移一天,防止出现负数
    
    if(utcToSeconds(clockTime,UTC_S_2000)>10*365*86400){
        _time_base=clockTime-_nowTick;//计算时钟基准
    }else{
        _time_base=0;
    }
            
    //设置随机数种
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
//    clock_now=parseTime(clockTime,0)+500; //转换时间增加500ms,平衡误差值
//    rtc_now=drv_Time_get();
//    
//    //出现秒级差异,执行修正
//    if( (clock_now>(rtc_now+999)) || (rtc_now>(999+clock_now)) ){
//        _time_base=clock_now-_nowTick;   
//    }
//}


void drv_Time_load(void){
#ifdef PIGEON_V7
    DATETIME clockTime;
    bool needVerify=false;
    
    taskDISABLE_INTERRUPTS();
    //每5秒核对一次
    if ( (_nowTick-_lastVerifyTick)>5000 ){
        needVerify=true;
        _lastVerifyTick=_nowTick+5000;
    }
    taskENABLE_INTERRUPTS();
    
    if(needVerify){
        if(!hal_DS3231_getTime(&clockTime)) return;  //异常,忽略
        
        taskDISABLE_INTERRUPTS();
        drv_Time_load_(&clockTime);
        taskENABLE_INTERRUPTS();
    }
#endif
}


void drv_Time_set(Int64 masterTime){
    //DS1302操作比较慢，所以不锁定OS,但一定要确保读写只在同一个任务中执行
#ifdef    PIGEON_V7
    { //写入到DS1302(UTC),
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


//如果对时间精度要求高，就调用这个函数，直接读取硬件时钟
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
//采用最高的任务优先级,所以一律不需要taskDISABLE_INTERRUPTS/taskENABLE_INTERRUPTS
void drv_TimeTick_callback( TimerHandle_t xTimer ){//软件定时器tick
    _nowTick=hal_RTC_getCount();//周期性刷新时间
    if(_nowTick > 0xFFFF0000){
        NVIC_SystemReset();  //连续运行40天才会执行到这里,仅用于防范系统BUG
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

