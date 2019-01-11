#ifndef  __CPU_H__
#define  __CPU_H__

void hal_CPU_config(void);
void hal_CPU_getId(UInt8* uid);
void drv_CPU_getDeviceId(void* deviceID);
#define hal_CPU_reset  NVIC_SystemReset
void drv_CPU_init(void);
#define drv_CPU_sleep        hal_CPU_sleep
#define drv_CPU_stop         hal_CPU_stop

void hal_IDOG_flush(void);
void drv_CPU_requestReset(UInt32 resetTick);
bool drv_CPU_checkReset(UInt32 now);

void isr_Event_raise(UInt32 flag);
UInt32 fns_Event_test(UInt32 flagMask);


void hal_Battery_config(void);
bool hal_Battery_isLower(void);
#endif
