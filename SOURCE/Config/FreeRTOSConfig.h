#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

//��ʱ������
#define configUSE_TIMERS                1
#define configTIMER_TASK_PRIORITY       (tskIDLE_PRIORITY+6)
#define configTIMER_QUEUE_LENGTH        100





#define configSUPPORT_DYNAMIC_ALLOCATION    0
#define configSUPPORT_STATIC_ALLOCATION     1  //һ���þ�̬����
#define configTIMER_TASK_STACK_DEPTH    100       //��ʱ�������ջ��С
#define configMINIMAL_STACK_SIZE	    100       //���������ջ��С


#define configUSE_PREEMPTION		1                                   //ʹ������ʽ�ں�
#define configUSE_IDLE_HOOK			0                                   //���й�����
#define configUSE_TICK_HOOK			0                                   //ʱ��Ƭ������
#define configCPU_CLOCK_HZ			( ( unsigned portLONG ) 72000000 )  //������ִ��Ƶ��
#define configTICK_RATE_HZ			( ( portTickType ) 1000 )           //ʱ��Ƭ�ж�Ƶ��
#define configMAX_PRIORITIES		7    //�������ȼ���Ŀ

//#define configTOTAL_HEAP_SIZE		( ( size_t ) 12*1024 )              //�ں˿���RAM����(�������ջ)20k
#define configMAX_TASK_NAME_LEN		8                                   //����������󳤶�(��NULL��ֹ�ֽ�)
#define configUSE_TRACE_FACILITY	0                                   //���ӻ�׷�ٹ���
#define configUSE_16_BIT_TICKS		0                                   //24 ���¼���־
#define configIDLE_SHOULD_YIELD		1                                   //�ӳٿ�������ִ��ʱ��



#define configUSE_MUTEXES               1                               //ʹ�û��⹦��
#define configUSE_RECURSIVE_MUTEXES     1                               //�ݹ黥�⹦��
#define configUSE_COUNTING_SEMAPHORES   1                               //���������ź���
#define configUSE_ALTERNATIVE_API       0                               //������к���
#define configCHECK_FOR_STACK_OVERFLOW  0                               //��ջ������

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0                                   //Эͬ����
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )                           //Эͬ�����п��õ����ȼ���Ŀ


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


//��KERNEL~SYSCALL֮����жϿ��Ե���FromISR����
//STM32�н���4λ��Ч
#define configKERNEL_INTERRUPT_PRIORITY 		0xFF     //ϵͳ�ں��ж����ȼ�,һ����������Ϊ���
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	0x8F     //���Ե���FromISR�������ж�������ȼ�

#endif
