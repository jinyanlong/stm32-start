#ifndef RACE_FUNC_H
#define RACE_FUNC_H


#include "FileFunc.h"
#include "SMD.h"
#include "RaceTask.h"

#define UPLOAD_OVERTIME 2000

void drv_Race_createSystem(UInt8* systemId,UInt32 systemMark,UInt8* pSysKey);
bool drv_Race_isSameSystem(UInt8* systemId,UInt8* pSysKey);
void drv_Race_createSystemRndB(void);
void drv_Race_setSystemRndA(UInt8* pRndA);
void drv_Race_getSystemRndB(UInt8* pRndB);
void drv_Race_getSystemMacA(UInt8* pMac);
bool drv_Race_verifySystemMacB(UInt8* pMac);
bool drv_Race_isSystemReady(void);

bool drv_Race_checkValidate(void);




















#endif
