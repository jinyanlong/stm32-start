#ifndef FUNC_PIGEON_H_001
#define FUNC_PIGEON_H_001

#include "FUNC_Common.h"


#define COMM_RACEINFO_MODE       0x00
#define COMM_RACEINFO_FLYTIME    0x01
#define COMM_RACEINFO_VALIDATE   0x02

#define FVINS_PING       0x10
#define FVINS_RECORD     0x11

//0x00,ÐÅÏ¢Çå³ý
void drv_CommFunc_clear(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommFunc_setSite(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommFunc_getApplyData(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommFunc_setApplyData(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommFunc_getSite(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);

void drv_CommFunc_getRecordInfo(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommFunc_getRecord(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);

void drv_CommFunc_lfSendData(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommFunc_nrfSendData(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);

void drv_Comm_sendPing(UartPacketHandler* pUartHandler);
void drv_Comm_sendRecord(UartPacketHandler* pUartHandler,UInt32 index);
void drv_Comm_sendRecords(UartPacketHandler* pUartHandler);


#endif
