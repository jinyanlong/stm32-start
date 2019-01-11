#ifndef SMD_H
#define SMD_H

#include "SFTypes.h"
#include "MessageLoop.h"
#include "RaceTask.h"


void drv_SMD_sendRegister(void);
void drv_SMD_sendPing(MONITOR* monitor);
void drv_SMD_sendRecords(UInt32 sof, UInt32 count);

//以下回调函数在RaceTask中执行
void drv_SMD_onRegister(HRESULT hr);
void drv_SMD_onPing(HRESULT hr);
void drv_SMD_onRecord(HRESULT hr);

#endif
