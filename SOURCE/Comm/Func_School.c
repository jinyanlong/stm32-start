#include "includes.h"
#include "Func_School.h"
#include "RealTimer.h"
#include "halLF.h"
#include "Func_Common.h"
#include "RealTimer.h"
#include "lf.h"
#include "flash.h"
#include "MainTask.h"

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

    // drv_Data_setSiteName(fvCmd->PARAMS);
    // drv_Data_flush();
}

// 天线电源号POWID(1B)+天线电源参数PPARA(1B)+天线1的Q值+天线2的Q值
void drv_CommFunc_setQVcc(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){

    if(fvCmd->LEN<4){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    // if(!_commVerify){
    //     drv_CommFunc_setError(fvRsp,ERRCODE_SECRITY_PERMIT);
    //     return;
    // }
    if(fvCmd->PARAMS[0]==MCP4018T_1){
        drv_LFset_LFQVAL(LF_ANTA,fvCmd->PARAMS[2]);
        drv_LFset_LFQVAL(LF_ANTB,fvCmd->PARAMS[3]);
        drv_SET_LFPOWVAL(MCP4018T_1,fvCmd->PARAMS[1]);
    }else if(fvCmd->PARAMS[0]==MCP4018T_2){
        drv_LFset_LFQVAL(LF_ANTM,fvCmd->PARAMS[2]);
        drv_LFset_LFQVAL(LF_ANTN,fvCmd->PARAMS[3]);
        drv_SET_LFPOWVAL(MCP4018T_2,fvCmd->PARAMS[1]); 
   }else{
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
   }
}
//返回：天线电源参数PPARA(1B)+天线1-Q值(1B)+天线2-Q值(1B)
void drv_CommFunc_getQVcc(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){

    if(fvCmd->LEN<1){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    // if(!_commVerify){
    //     drv_CommFunc_setError(fvRsp,ERRCODE_SECRITY_PERMIT);
    //     return;
    // }
    if(fvCmd->PARAMS[0]==MCP4018T_1){
        fvRsp->PARAMS[0]=drv_GET_LFPOWVAL(MCP4018T_1);
        fvRsp->PARAMS[1]=fns_Flash_getQVAL(LF_ANTA);
        fvRsp->PARAMS[2]=fns_Flash_getQVAL(LF_ANTB);
    }else if(fvCmd->PARAMS[0]==MCP4018T_2){
        fvRsp->PARAMS[0]=drv_GET_LFPOWVAL(MCP4018T_2);
        fvRsp->PARAMS[1]=fns_Flash_getQVAL(LF_ANTM);
        fvRsp->PARAMS[2]=fns_Flash_getQVAL(LF_ANTN);
   }else{
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
   }
   fvRsp->LEN=3;
}

void drv_CommFunc_adjustLFC(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    if(fvCmd->LEN<1){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    // if(!_commVerify){
    //     drv_CommFunc_setError(fvRsp,ERRCODE_SECRITY_PERMIT);
    //     return;
    // }
    if((fvCmd->PARAMS[0]>0)&&(fvCmd->PARAMS[0]<5)){
        drv_adjust_LFCap(fvCmd->PARAMS[0]);
    }else{
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
    }
}
void drv_CommFunc_getLFC(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    if(fvCmd->LEN<1){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    // if(!_commVerify){
    //     drv_CommFunc_setError(fvRsp,ERRCODE_SECRITY_PERMIT);
    //     return;
    // }
    if((fvCmd->PARAMS[0]>0)&&(fvCmd->PARAMS[0]<5)){
        fvRsp->PARAMS[0]=fns_Flash_getLFC(fvCmd->PARAMS[0]);
        fvRsp->LEN=1;
    }else{
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
    }
}
// 天线位置(1B)+保留(1B)+天线号（2B）
void drv_CommFunc_setANTId(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    UInt16 value;
    if(fvCmd->LEN<4){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    // if(!_commVerify){
    //     drv_CommFunc_setError(fvRsp,ERRCODE_SECRITY_PERMIT);
    //     return;
    // }
    memcpy(&value,fvCmd->PARAMS+2,2);
    fns_Flash_setAntId(fvCmd->PARAMS[0],value);
}

void drv_CommFunc_getANTId(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    UInt16 value;
    if(fvCmd->LEN<2){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    // if(!_commVerify){
    //     drv_CommFunc_setError(fvRsp,ERRCODE_SECRITY_PERMIT);
    //     return;
    // }
    value=fns_Flash_getAntId(fvCmd->PARAMS[0]);
    memcpy(fvRsp->PARAMS,&value,2);
    fvRsp->LEN=2;
}

void drv_CommFunc_getApplyData(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    if(fvCmd->LEN<4){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    } 
    // switch(fvCmd->PARAMS[0]){
    // case FVAPPLY_DEVICENAME:
    // {
    //     UTFCHARS site;
    //     drv_Data_getSiteName(&site);
        
    //     fvRsp->LEN=2+site.length*2;
    //     memcpy(fvRsp->PARAMS,&site,fvRsp->LEN);
    //     break;
    // }
    // case FVAPPLY_NETIP:
    //     drv_Data_getAString(ASTRING_TYPE_IP,(AStringBuffer)&fvRsp->PARAMS);
    //     fvRsp->LEN=1+fvRsp->PARAMS[0];
    //     break;
    // case FVAPPLY_NETPATH:
    //     drv_Data_getAString(ASTRING_TYPE_DIR,(AStringBuffer)&fvRsp->PARAMS);
    //     fvRsp->LEN=1+fvRsp->PARAMS[0];
    //     break;
    // case FVAPPLY_APN:
    //     drv_Data_getAString(ASTRING_TYPE_APN,(AStringBuffer)&fvRsp->PARAMS);
    //     fvRsp->LEN=1+fvRsp->PARAMS[0];
    //     break;
    // case FVAPPLY_WEBPING:
    //     memcpy(fvRsp->PARAMS,(void*)&s_monitor.webPingInterval,2);
    //     fvRsp->LEN=2;
    //     break;
    // case FVAPPLY_TAGPING:
    //     memcpy(fvRsp->PARAMS,(void*)&s_monitor.tagPingInterval,2);
    //     fvRsp->LEN=2;
    //     break;
    // case FVAPPLY_TAGOFFTIME:
    //     memcpy(fvRsp->PARAMS,(void*)&s_monitor.tagOfftime,2);
    //     fvRsp->LEN=2;
    //     break;
    // case FVAPPLY_UARTPING:
    //     memcpy(fvRsp->PARAMS,(void*)&s_monitor.uartPingInterval,2);
    //     fvRsp->LEN=2;
    //     break;
    // case FVAPPLY_UARTOVERTIME:
    //     memcpy(fvRsp->PARAMS,(void*)&s_monitor.uartOvertime,2);
    //     fvRsp->LEN=2;
    //     break;
    // default:
    //     drv_CommFunc_setError(fvRsp,ERRCODE_UNSUPPORT);
    //     break;
    // }

}
// void drv_CommFunc_setApplyData(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
//     fns_SetPcComm_State(PCCOMM_STATE_ACK);
//     if(fvCmd->LEN<5){
//         drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
//         return;
//     }
    
//     //权限检查，设置参数前需要初始化或校验密码
//     if(!_commVerify){
//         drv_CommFunc_setError(fvRsp,ERRCODE_SECRITY_PERMIT);
//         return;
//     }

//     switch(fvCmd->PARAMS[0]){
//     case FVAPPLY_DEVICENAME:
//         drv_CommFunc_setSite(fvCmd,fvRsp);
//         break;
//     case FVAPPLY_NETIP:            
//         drv_Data_setAString(ASTRING_TYPE_IP,(AString)(fvCmd->PARAMS+4)); //写入缓存
//         drv_Data_flush(); 
//         break;
//     case FVAPPLY_NETPATH:
//         drv_Data_setAString(ASTRING_TYPE_DIR,(AString)(fvCmd->PARAMS+4)); //写入缓存
//         drv_Data_flush(); 
//         break;
//     case FVAPPLY_APN:
//         drv_Data_setAString(ASTRING_TYPE_APN,(AString)(fvCmd->PARAMS+4)); //写入缓存
//         drv_Data_flush();
//         break;
//     case FVAPPLY_WEBPING:
//         memcpy((void*)&s_monitor.webPingInterval,fvCmd->PARAMS+4,2);
//         drv_Data_setWebPingInterval(s_monitor.webPingInterval);
//         drv_Data_flush();
//         break;
//     case FVAPPLY_TAGPING:
//         memcpy((void*)&s_monitor.tagPingInterval,fvCmd->PARAMS+4,2);
//         drv_Data_setTagPingInterval(s_monitor.tagPingInterval);
//         drv_Data_flush();
//         break;
//     case FVAPPLY_TAGOFFTIME:
//         memcpy((void*)&s_monitor.tagOfftime,fvCmd->PARAMS+4,2);
//         drv_Data_setTagOfftime(s_monitor.tagOfftime);
//         drv_Data_flush();
//         break;
//     case FVAPPLY_UARTPING:
//         memcpy((void*)&s_monitor.uartPingInterval,fvCmd->PARAMS+4,2);
//         drv_Data_setUartPingInterval(s_monitor.uartPingInterval);
//         drv_Data_flush();
//         break;
//     case FVAPPLY_UARTOVERTIME:
//         memcpy((void*)&s_monitor.uartOvertime,fvCmd->PARAMS+4,2);
//         drv_Data_setUartOvertime(s_monitor.uartOvertime);
//         drv_Data_flush();
//         break;
//     default:
//         drv_CommFunc_setError(fvRsp,ERRCODE_UNSUPPORT);
//         break;
//     }
// }

extern volatile MONITOR s_monitor;


void drv_CommFunc_lfSendDataNoAck(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    UInt16 sendData;
    if(fvCmd->LEN<2){
        drv_CommFunc_setError(fvRsp,ERRCODE_PACKET_LEN);
        return;
    }
    memcpy(&sendData,fvCmd->PARAMS,2);
    drv_LF_send(sendData);
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
    drv_LF_send(cmdData);

    _pcCommUart.rspTimout=drv_Time_getTick()+500;

}
//2.4G指令转发
void drv_CommFunc_nrfSendData(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    drv_CommFunc_setError(fvRsp,ERRCODE_UNSUPPORT);
    // if(fvCmd->LEN<22){
    //     drv_CommFunc_setError(fvRsp,ERRCODE_PACKET_LEN);
    //     return;
    // }
    // _pcCommUart.rspINS=(fvCmd->PARAMS[3]);
    // drv_Nrf_sendData(fvCmd->PARAMS,fvCmd->LEN);

    // _pcCommUart.rspTimout=drv_Time_getTick()+500;
    // _pcCommUart.lazyMode=true;
}


void drv_CommFunc_nrfTest(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    drv_CommFunc_setError(fvRsp,ERRCODE_UNSUPPORT);
   
    // drv_Nrf_sendTest();

    // _pcCommUart.rspTimout=drv_Time_getTick()+100;
    // _pcCommUart.lazyMode=true;    

}




