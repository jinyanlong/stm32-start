#include "includes.h"
#include "CommDriver.h"
#include "Func_Common.h"
#include "Func_School.h"


///////////////////////////设备常用指令////////////////////////////////////
void (*FUNC_COMMON[])(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp)={
    drv_CommonFunc_getAttr,	         //0
    drv_CommonFunc_setAttr,          //1
    drv_CommonFunc_formatDevice,     //2 恢复出厂设置
    drv_CommonFunc_beep,	         //3
    drv_CommonFunc_readDevice,       //4
    drv_CommonFunc_writeDevice,      //5
    NULL,// drv_CommonFunc_readIO,   //6
    NULL,// drv_CommonFunc_writeIO,  //7
	drv_CommonFunc_reBootDevice,      //8
    drv_CommonFunc_light
};

///////////////////////////设备安全指令////////////////////////////////////
void (*FUNC_SECRITY[])(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp)={
    drv_SecrityFunc_GetAttr,	     //0
    drv_SecrityFunc_SetAttr,	     //1
    fns_SecrityFunc_init,	         //2
    NULL,                            //3
    NULL,
    fns_SecrityFunc_verify,         //5,校验密码
    NULL// drv_CommFunc_clear              //6,信息清除
};


/////////////////////主接口////////////////////////////
void (*drv_CommFunc_Command[])(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp)={
    drv_CommFunc_setQVcc,
    drv_CommFunc_adjustLFC,
    drv_CommFunc_setANTId
};


void (*drv_CommFunc_Notify[])(FV_COMMAND* fvCmd)={
    NULL,
    NULL,
    NULL,
    NULL
    // drv_CommFunc_ackRecord
};


////////////////////////////////////////////////////////////////////////
void drv_Comm_onCommand(FV_COMMAND* cmdPacket,FV_RESPONSE* rspPacket){
    rspPacket->CLA=cmdPacket->CLA;
    FVCLA_SETTYPE(rspPacket->CLA,FVTYPE_ACK);
    rspPacket->INS=cmdPacket->INS;
    rspPacket->LEN=0;
    
    switch( FVCLA_GETID(cmdPacket->CLA) ){
    case FVCLA_DEVICE:  //常用指令
        drv_CommFunc_call(FUNC_COMMON,cmdPacket->INS,sizeof(FUNC_COMMON)/sizeof(FUNC_COMMON[0]),cmdPacket,rspPacket);
        break;
    case FVCLA_SYSTEM:
        drv_CommFunc_call(FUNC_SECRITY,cmdPacket->INS,sizeof(FUNC_SECRITY)/sizeof(FUNC_SECRITY[0]),cmdPacket,rspPacket);
        break;   
    case FVCLA_SCHOOL:  //参数设置
        drv_CommFunc_call(drv_CommFunc_Command,cmdPacket->INS,sizeof(drv_CommFunc_Command)/sizeof(drv_CommFunc_Command[0]),cmdPacket,rspPacket);
        break;
    default:
        drv_CommFunc_setError(rspPacket,ERRCODE_UNSUPPORT);
        break;
    }
}

void drv_Comm_onNotify(FV_COMMAND* cmdPacket){
    switch( FVCLA_GETID(cmdPacket->CLA) ){   
    case FVCLA_SCHOOL:
        drv_CommFunc_callNotify(drv_CommFunc_Notify,cmdPacket->INS,sizeof(drv_CommFunc_Notify)/sizeof(drv_CommFunc_Notify[0]),cmdPacket);
        break;
    default:
        //忽略
        break;
    }
}





