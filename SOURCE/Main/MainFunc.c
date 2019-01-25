#include "includes.h"
#include "MainFunc.h"

extern UInt8 s_systemId[4];
extern UInt8 s_systemKey[8];
//У��ϵͳ����
bool drv_Main_isSameSystem(UInt8* systemId,UInt8* pSysKey){
    if(memcmp(systemId,s_systemId,4)!=0){
        return false;
    }
    if(memcmp(pSysKey,s_systemKey,8)!=0){
        return false;
    }
    return true;
}

