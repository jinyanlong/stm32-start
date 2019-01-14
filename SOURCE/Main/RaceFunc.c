#include "includes.h"
#include "halCPU.h"
#include "SFile.h"
#include "RaceFunc.h"
#include "RealTimer.h"
#include "Form.h"
#include "RealTimer.h"

extern UInt8  ROM_WORKMODE;
extern UInt8  ROM_PROXYID[2];
extern UInt32 s_deviceID;
extern UInt8 s_systemId[4];
extern UInt32 s_systemMark;

extern UInt8 s_systemKey[];
extern UInt8 _systemRnd[];
extern DESKEYSTRUCT desKey; //desKey实际上是局部变量,提升至全局,是为了减少栈空间消耗

extern const SFILE s_dataFile;
extern volatile MONITOR s_monitor;

void drv_Race_createSystem(UInt8* systemId,UInt32 systemMark,UInt8* pSysKey){
    memcpy(s_systemId,systemId,4);
    s_systemMark=systemMark;
    memcpy(s_systemKey,pSysKey,8);

    drv_Data_reset();
    drv_Data_setSystem(s_systemId,systemMark,s_systemKey);
}

//校验系统密码
bool drv_Race_isSameSystem(UInt8* systemId,UInt8* pSysKey){
    if(memcmp(systemId,s_systemId,4)!=0){
        return false;
    }
    if(memcmp(pSysKey,s_systemKey,8)!=0){
        return false;
    }
    return true;
}



//产生4B的随机数
void drv_Race_createSystemRndB(void){
    UInt8 tm[8];
    printTimeEx(fns_Time_get(),0,(DATETIME_EX*)tm);
    _systemRnd[4]=tm[0];
    _systemRnd[5]=tm[1]*_systemRnd[4];
    _systemRnd[6]=tm[2]*_systemRnd[5];
    _systemRnd[7]=tm[3]*_systemRnd[6];
}
void drv_Race_setSystemRndA(UInt8* pRndA){
    _systemRnd[0]=pRndA[0];
    _systemRnd[1]=pRndA[1];
    _systemRnd[2]=pRndA[2];
    _systemRnd[3]=pRndA[3];
}

void drv_Race_getSystemRndB(UInt8* pRndB){
    memcpy(pRndB,_systemRnd+4,4);
}
//用_systemKey 解密 id+随机数
void drv_Race_getSystemMacA(UInt8* pMac){
    UInt8 databuff[8];
    memcpy(databuff,s_systemId,4);
    memcpy(databuff+4,_systemRnd,4);
    
    drv_DES_createSubKey(s_systemKey,&desKey);
    drv_DES_decrypt(databuff,databuff,&desKey);
    memcpy(pMac,databuff,4);
}
  
bool drv_Race_verifySystemMacB(UInt8* pMac){
    UInt8 databuff[8];
    memcpy(databuff,s_systemId,4);
    memcpy(databuff+4,_systemRnd+4,4);
    
    drv_DES_createSubKey(s_systemKey,&desKey);
    drv_DES_decrypt(databuff,databuff,&desKey);
    if(  memcmp(pMac,databuff,4)==0 ){
        return true;
    }else{
        return false;
    }
}

bool drv_Race_isSystemReady(void){
    if( (s_systemId[0]==ROM_PROXYID[0]) && (s_systemId[1]==ROM_PROXYID[1]) ){
        return true;
    }else{
        return false;
    }
}

bool drv_Race_checkValidate(void){
    if(s_monitor.validate && (s_monitor.validate<fns_Time_get()) ){
        return false;
    }else{
        return true;
    }
}

