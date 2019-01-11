//通讯帧结构
#ifndef __PCCOMM_H
#define __PCCOMM_H
#include "CommDriver.h"
void hal_PcComm_config(void);
void drv_PcComm_CheckState(UInt32 nowTick);
void drv_PcComm_onEvent(void);

#endif
