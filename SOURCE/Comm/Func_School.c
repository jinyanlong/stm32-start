#include "includes.h"
#include "Func_School.h"
#include "RaceFunc.h"
#include "stringRes.h"
#include "font.h"
#include "RealTimer.h"
#include "RaceTask.h"
#include "RaceData.h"
#include "lf.h"
#include "halNrf.h"
#include "Func_Common.h"
#include "Nrf.h"
#include "RealTimer.h"

extern bool _commVerify;
extern volatile MONITOR s_monitor;
extern UInt32 s_systemMark;
extern UInt32 s_deviceID;
extern UInt8 s_systemId[4];

volatile UInt8 _pcCommState=0;
volatile UInt32 _rspTimout=0;
////////////////////////////////////////////////////////////////////
//大部分通讯函数直接操作flash,通讯结束后必须复位退出,以确保内存与Flash同步
////////////////////////////////////////////////////////////////////

//0x00,信息清除
void drv_CommFunc_clear(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    //参数检查
    if(fvCmd->LEN<1){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    //权限检查
    if(!_commVerify){
        drv_CommFunc_setError(fvRsp,ERRCODE_SECRITY_PERMIT);
        return;
    }
    drv_Data_clear(fvCmd->PARAMS[0]);
    drv_Data_flush();

    drv_RaceTask_load();
}

void drv_CommFunc_setSite(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    //参数检查
    if(fvCmd->LEN<6){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    //权限检查
    if(!_commVerify){
        drv_CommFunc_setError(fvRsp,ERRCODE_SECRITY_PERMIT);
        return;
    }

    drv_Data_setSiteName(fvCmd->PARAMS);
    drv_Data_flush();
}

void drv_CommFunc_getApplyData(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    fns_SetPcComm_State(PCCOMM_STATE_ACK);

    if(fvCmd->LEN<4){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    } 
    switch(fvCmd->PARAMS[0]){
    case FVAPPLY_DEVICENAME:
    {
        UTFCHARS site;
        drv_Data_getSiteName(&site);
        
        fvRsp->LEN=2+site.length*2;
        memcpy(fvRsp->PARAMS,&site,fvRsp->LEN);
        break;
    }
    case FVAPPLY_NETIP:
        drv_Data_getAString(ASTRING_TYPE_IP,(AStringBuffer)&fvRsp->PARAMS);
        fvRsp->LEN=1+fvRsp->PARAMS[0];
        break;
    case FVAPPLY_NETPATH:
        drv_Data_getAString(ASTRING_TYPE_DIR,(AStringBuffer)&fvRsp->PARAMS);
        fvRsp->LEN=1+fvRsp->PARAMS[0];
        break;
    case FVAPPLY_APN:
        drv_Data_getAString(ASTRING_TYPE_APN,(AStringBuffer)&fvRsp->PARAMS);
        fvRsp->LEN=1+fvRsp->PARAMS[0];
        break;
    case FVAPPLY_WEBPING:
        memcpy(fvRsp->PARAMS,(void*)&s_monitor.webPingInterval,2);
        fvRsp->LEN=2;
        break;
    case FVAPPLY_TAGPING:
        memcpy(fvRsp->PARAMS,(void*)&s_monitor.tagPingInterval,2);
        fvRsp->LEN=2;
        break;
    case FVAPPLY_TAGOFFTIME:
        memcpy(fvRsp->PARAMS,(void*)&s_monitor.tagOfftime,2);
        fvRsp->LEN=2;
        break;
    case FVAPPLY_UARTPING:
        memcpy(fvRsp->PARAMS,(void*)&s_monitor.uartPingInterval,2);
        fvRsp->LEN=2;
        break;
    case FVAPPLY_UARTOVERTIME:
        memcpy(fvRsp->PARAMS,(void*)&s_monitor.uartOvertime,2);
        fvRsp->LEN=2;
        break;
    default:
        drv_CommFunc_setError(fvRsp,ERRCODE_UNSUPPORT);
        break;
    }

}
void drv_CommFunc_setApplyData(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    fns_SetPcComm_State(PCCOMM_STATE_ACK);
    if(fvCmd->LEN<5){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    
    //权限检查，设置参数前需要初始化或校验密码
    if(!_commVerify){
        drv_CommFunc_setError(fvRsp,ERRCODE_SECRITY_PERMIT);
        return;
    }

    switch(fvCmd->PARAMS[0]){
    case FVAPPLY_DEVICENAME:
        drv_CommFunc_setSite(fvCmd,fvRsp);
        break;
    case FVAPPLY_NETIP:            
        drv_Data_setAString(ASTRING_TYPE_IP,(AString)(fvCmd->PARAMS+4)); //写入缓存
        drv_Data_flush(); 
        break;
    case FVAPPLY_NETPATH:
        drv_Data_setAString(ASTRING_TYPE_DIR,(AString)(fvCmd->PARAMS+4)); //写入缓存
        drv_Data_flush(); 
        break;
    case FVAPPLY_APN:
        drv_Data_setAString(ASTRING_TYPE_APN,(AString)(fvCmd->PARAMS+4)); //写入缓存
        drv_Data_flush();
        break;
    case FVAPPLY_WEBPING:
        memcpy((void*)&s_monitor.webPingInterval,fvCmd->PARAMS+4,2);
        drv_Data_setWebPingInterval(s_monitor.webPingInterval);
        drv_Data_flush();
        break;
    case FVAPPLY_TAGPING:
        memcpy((void*)&s_monitor.tagPingInterval,fvCmd->PARAMS+4,2);
        drv_Data_setTagPingInterval(s_monitor.tagPingInterval);
        drv_Data_flush();
        break;
    case FVAPPLY_TAGOFFTIME:
        memcpy((void*)&s_monitor.tagOfftime,fvCmd->PARAMS+4,2);
        drv_Data_setTagOfftime(s_monitor.tagOfftime);
        drv_Data_flush();
        break;
    case FVAPPLY_UARTPING:
        memcpy((void*)&s_monitor.uartPingInterval,fvCmd->PARAMS+4,2);
        drv_Data_setUartPingInterval(s_monitor.uartPingInterval);
        drv_Data_flush();
        break;
    case FVAPPLY_UARTOVERTIME:
        memcpy((void*)&s_monitor.uartOvertime,fvCmd->PARAMS+4,2);
        drv_Data_setUartOvertime(s_monitor.uartOvertime);
        drv_Data_flush();
        break;
    default:
        drv_CommFunc_setError(fvRsp,ERRCODE_UNSUPPORT);
        break;
    }
}

extern volatile MONITOR s_monitor;
void drv_CommFunc_getRecordInfo(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    fns_SetPcComm_State(PCCOMM_STATE_ACK);
    if(s_monitor.totalSOF==s_monitor.totalEOF){
        //内部无记录
        memset(fvRsp->PARAMS,0,16);
    }else{
        UInt32 recordId;
        recordId=s_monitor.totalSOF+1;
        memcpy(fvRsp->PARAMS,&recordId,4);
        
        recordId=s_monitor.totalEOF;
        memcpy(fvRsp->PARAMS+4,&recordId,4);
    }
    fvRsp->LEN=16;
}

//标志(1B)+00H+天线ID(2B)+卡号(4B)+步数(4B)+记录时间(4B,秒)+触发时间(8B,LINUX-UTC时间)
void drv_CommFunc_getRecord(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    UInt32 recordId;
    TAGRECORD tagRecord;
    fns_SetPcComm_State(PCCOMM_STATE_ACK);
    memcpy(&recordId,fvCmd->PARAMS,4);
    if(recordId==0){
        drv_CommFunc_setError(fvRsp,ERRCODE_PACKET_PARAMS);
        return;
    }
    
    if( (recordId <=s_monitor.totalSOF ) || (recordId>=s_monitor.totalEOF) ){
        //记录已删除,返回空,不报错
        return;
    }
    
    drv_Record_get(recordId-1,&tagRecord);
    fvRsp->PARAMS[0]=tagRecord.flag;
    fvRsp->PARAMS[1]=0;
    memcpy(fvRsp->PARAMS+2,&tagRecord.coinId,2);
    memcpy(fvRsp->PARAMS+4,&tagRecord.tagId,4);
    memcpy(fvRsp->PARAMS+8,&tagRecord.steper,4);
    
    {
        UInt32 saveTime=utcToSeconds(TICK2UTC(tagRecord.baseTime,tagRecord.saveTick),UTC_S_2000); 
        memcpy(fvRsp->PARAMS+12,&saveTime,4);
    }
        
    {
        Int64 raiseTime;
        raiseTime=TICK2UTC(tagRecord.baseTime,tagRecord.raiseTick);
        memcpy(fvRsp->PARAMS+16,&raiseTime,8);
    }
    fvRsp->LEN=24;
}


void drv_CommFunc_lfSendDataNoAck(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    UInt16 sendData;
    if(fvCmd->LEN<2){
        drv_CommFunc_setError(fvRsp,ERRCODE_PACKET_LEN);
        return;
    }
    memcpy(&sendData,fvCmd->PARAMS,2);
    if(!(lfSendData(sendData))){
        drv_CommFunc_setError(fvRsp,ERRCODE_LF_BUSY);
    }
}
extern UartPacketHandler _pcCommUart;
//低频指令转发
void drv_CommFunc_lfSendData(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    UInt16 cmdData;
    if(fvCmd->LEN<2){
        drv_CommFunc_setError(fvRsp,ERRCODE_PACKET_LEN);
        return;
    }
    memcpy(&cmdData,fvCmd->PARAMS,2);
    _pcCommUart.rspINS=(cmdData>>8);
    if(!(lfSendData(cmdData))){
        drv_CommFunc_setError(fvRsp,ERRCODE_LF_BUSY);
        return;
    }

    _pcCommUart.rspTimout=drv_Time_getTick()+500;
    _pcCommUart.lazyMode=true;

}
//2.4G指令转发
void drv_CommFunc_nrfSendData(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    if(fvCmd->LEN<22){
        drv_CommFunc_setError(fvRsp,ERRCODE_PACKET_LEN);
        return;
    }
    _pcCommUart.rspINS=(fvCmd->PARAMS[3]);
    drv_Nrf_sendData(fvCmd->PARAMS,fvCmd->LEN);

    _pcCommUart.rspTimout=drv_Time_getTick()+500;
    _pcCommUart.lazyMode=true;
}


void drv_CommFunc_nrfTest(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    drv_Nrf_sendTest();

    _pcCommUart.rspTimout=drv_Time_getTick()+100;
    _pcCommUart.lazyMode=true;    

}

void drv_CommFunc_ackRecord(FV_COMMAND* cmdPacket){
    UInt32 sysMark;

    UInt32 recordId;
    if(cmdPacket->LEN<8){
        return;
    }
    memcpy(&sysMark,cmdPacket->PARAMS,4);
    memcpy(&recordId,cmdPacket->PARAMS+4,4);
    
    //MARK不匹配
    if(sysMark!=0){
        if(sysMark!=s_systemMark) return;
    }
    
    if(recordId==0){
        s_monitor.uartUploadSOF=s_monitor.totalSOF;
        drv_Data_setUartUploadSOF(s_monitor.uartUploadSOF);
        drv_Data_flush();
        return;
    }
    if(recordId>s_monitor.totalEOF){ //异常
        return;
    }
    
    if( recordId<=s_monitor.totalSOF){
        s_monitor.uartUploadSOF=s_monitor.totalSOF;
    }else{
        s_monitor.uartUploadSOF=recordId;
    }
    


    drv_Data_setUartUploadSOF(s_monitor.uartUploadSOF);
    drv_Data_flush();
}




//PARAMS=设备时钟(8B)+设备编号(4B)+系统ID(4B)+系统MARK(4B)+记录起始ID(4B)+结束ID(4B)+已上传ID(4B)
void drv_Comm_sendPing(UartPacketHandler* pUartHandler){
    UInt64 devTime;
    UInt32 recordId; 
    FV_RESPONSE* sendMsg=&pUartHandler->rspPacket;
    UInt8 protocol=0x80; //无地址模式
    memset(sendMsg,0,sizeof(FV_RESPONSE));

    sendMsg->CLA=FVCLA_SCHOOL_MSG;
    sendMsg->INS=FVINS_PING;
    devTime=fns_Time_get();
	memcpy(sendMsg->PARAMS,&devTime,8);
    memcpy(sendMsg->PARAMS+8,&s_deviceID,4);
    memcpy(sendMsg->PARAMS+12,&s_systemId,4);
    memcpy(sendMsg->PARAMS+16,&s_systemMark,4);
    
    if(s_monitor.totalSOF<s_monitor.totalEOF){
        recordId=s_monitor.totalSOF+1;
        memcpy(sendMsg->PARAMS+20,&recordId,4);
        
        recordId=s_monitor.totalEOF;
        memcpy(sendMsg->PARAMS+24,&recordId,4);
        
        recordId=s_monitor.uartUploadSOF+1;
        memcpy(sendMsg->PARAMS+28,&recordId,4);
    }
    sendMsg->LEN=32;
    drv_Comm_sendPacket(pUartHandler->pUart,&protocol,sendMsg);
}

//  PARAMS=设备编号(4B)+系统ID(4B)+系统MARK(4B)+记录ID(4B)+记录标志(1B)+00H+天线ID(2B)
// +标签编号(4B)+步数(4B)+记录时间(4B,2000年开始的秒数)+触发时间(8B,UTC毫秒数)
void drv_Comm_sendRecord(UartPacketHandler* pUartHandler,UInt32 index){
    TAGRECORD tagRecord;
    FV_RESPONSE* sendMsg=&pUartHandler->rspPacket;
    UInt32 recId= index+1;
    UInt8 protocol=0x80; //无地址模式
    
    if( (index<s_monitor.totalSOF) || (index>=s_monitor.totalEOF) ){
        return; //忽略
    }
    
    memset(sendMsg,0,sizeof(FV_RESPONSE));
    sendMsg->CLA=FVCLA_SCHOOL_MSG;
    sendMsg->INS=FVINS_RECORD;
    memcpy(sendMsg->PARAMS,&s_deviceID,4);
    memcpy(sendMsg->PARAMS+4,s_systemId,4);    
    memcpy(sendMsg->PARAMS+8,&s_systemMark,4);
	memcpy(sendMsg->PARAMS+12,&recId,4);
    drv_Record_get(index,&tagRecord);
    sendMsg->PARAMS[16]=tagRecord.flag;
    sendMsg->PARAMS[17]=0;
    memcpy(sendMsg->PARAMS+18,&tagRecord.coinId,2);
    memcpy(sendMsg->PARAMS+20,&tagRecord.tagId,4);
    memcpy(sendMsg->PARAMS+24,&tagRecord.steper,4);
    
    {
        UInt32 saveTime=utcToSeconds(TICK2UTC(tagRecord.baseTime,tagRecord.saveTick),UTC_S_2000); 
        memcpy(sendMsg->PARAMS+28,&saveTime,4);
    }
    {
        Int64 raiseTime;
        raiseTime=TICK2UTC(tagRecord.baseTime,tagRecord.raiseTick);
        memcpy(sendMsg->PARAMS+32,&raiseTime,8);
    }
    sendMsg->LEN=40;
    
    drv_Comm_sendPacket(pUartHandler->pUart,&protocol,sendMsg);
#ifdef __RS485
    drv_Comm_sendPacket(pUartHandler->pUartRS485,&protocol,sendMsg);
#endif
}
//  PARAMS=设备编号(4B)+系统ID(4B)+系统MARK(4B)+记录ID(4B)+记录标志(1B)+00H+天线ID(2B)+标签编号(4B)+步数(4B)+记录时间(4B,2000年开始的秒数)+触发时间(8B,UTC毫秒数)
void drv_Comm_sendRecords(UartPacketHandler* pUartHandler){
    UInt32 i;
    if(s_monitor.uartUploadSOF< s_monitor.totalSOF){
        s_monitor.uartUploadSOF=s_monitor.totalSOF;
    }
    for(i=0;i<5;i++){
        if(s_monitor.uartUploadSOF < s_monitor.totalEOF ){
            drv_Comm_sendRecord(pUartHandler,s_monitor.uartUploadSOF);
            s_monitor.uartUploadSOF++;
            if(s_monitor.uartUploadSOF==s_monitor.totalEOF){
                drv_Data_setUartUploadSOF(s_monitor.uartUploadSOF);
                drv_Data_flush();
            }
        }
    }
}

void fns_SetPcComm_State(UInt8 data){
    _pcCommState=data;
}
UInt8 fns_GetPcComm_State(void){
    return _pcCommState;
}
