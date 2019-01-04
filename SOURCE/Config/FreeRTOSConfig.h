#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

//软定时器配置
#define configUSE_TIMERS                1
#define configTIMER_TASK_PRIORITY       (tskIDLE_PRIORITY+6)
#define configTIMER_QUEUE_LENGTH        100





#define configSUPPORT_DYNAMIC_ALLOCATION    0
#define configSUPPORT_STATIC_ALLOCATION     1  //一律用静态分配
#define configTIMER_TASK_STACK_DEPTH    100       //定时器任务堆栈大小
#define configMINIMAL_STACK_SIZE	    100       //空闲任务堆栈大小


#define configUSE_PREEMPTION		1                                   //使用抢先式内核
#define configUSE_IDLE_HOOK			0                                   //空闲钩函数
#define configUSE_TICK_HOOK			0                                   //时间片钩函数
#define configCPU_CLOCK_HZ			( ( unsigned portLONG ) 72000000 )  //处理器执行频率
#define configTICK_RATE_HZ			( ( portTickType ) 1000 )           //时间片中断频率
#define configMAX_PRIORITIES		7    //可用优先级数目

//#define configTOTAL_HEAP_SIZE		( ( size_t ) 12*1024 )              //内核可用RAM数量(含任务堆栈)20k
#define configMAX_TASK_NAME_LEN		8                                   //任务名称最大长度(含NULL终止字节)
#define configUSE_TRACE_FACILITY	0                                   //可视化追踪功能
#define configUSE_16_BIT_TICKS		0                                   //24 个事件标志
#define configIDLE_SHOULD_YIELD		1                                   //延迟空闲任务执行时间



#define configUSE_MUTEXES               1                               //使用互斥功能
#define configUSE_RECURSIVE_MUTEXES     1                               //递归互斥功能
#define configUSE_COUNTING_SEMAPHORES   1                               //计数器型信号量
#define configUSE_ALTERNATIVE_API       0                               //替代队列函数
#define configCHECK_FOR_STACK_OVERFLOW  0                               //堆栈溢出检测

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0                                   //协同例程
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )                           //协同例程中可用的优先级数目


/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */


#define INCLUDE_xTimerPendFunctionCall  1
#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				0
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1


//在KERNEL~SYSCALL之间的中断可以调用FromISR函数
//STM32中仅高4位有效
#define configKERNEL_INTERRUPT_PRIORITY 		0xFF     //系统内核中断优先级,一般总是设置为最低
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	0x8F     //可以调用FromISR函数的中断最高优先级

#endif
