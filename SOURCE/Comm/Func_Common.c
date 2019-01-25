#include "includes.h"
#include "halCPU.h"
#include "Func_Common.h"
#include "flash.h"
#include "RealTimer.h"
#include "CommDriver.h"
#include "MainFunc.h"
#include "Led.h"
#include "MainTask.h"

extern UInt8  ROM_WORKMODE;
extern UInt8  ROM_PROXYID[2];


bool _commVerify=false;
extern UInt8 s_systemId[4];
extern UInt32 s_systemMark;
extern UInt32 s_deviceID;
extern volatile MONITOR s_monitor;
////////////////////////////////辅助函数////////////////////////////////////////
void drv_CommFunc_reserved(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    drv_CommFunc_setError(fvRsp,ERRCODE_UNSUPPORT);
}


void drv_CommFunc_call(FVCMD_FUNCLIST pFunc,int index,int nMaxCount,FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    if( index >= nMaxCount ){
        drv_CommFunc_reserved(fvCmd,fvRsp);
        return;
    }

    if(NULL == pFunc[index]){
        drv_CommFunc_reserved(fvCmd,fvRsp);
        return;
    }

    (*pFunc[index])(fvCmd,fvRsp);
}

void drv_CommFunc_callNotify(FVNOTIFY_FUNCLIST pFunc,int index,int nMaxCount,FV_COMMAND* fvCmd){
    if( index >= nMaxCount ){
        return;
    }

    if(NULL == pFunc[index]){
        return;
    }

    (*pFunc[index])(fvCmd);
}

//////////////////////////////////////通用指令///////////////////////////////////
void drv_CommonFunc_getAttr(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    if(fvCmd->LEN<4){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    switch(fvCmd->PARAMS[0]){
    case FVATTR_STATE:
        memset(fvRsp->PARAMS,0,4); //此处可以返回需要监控的设备状态
        fvRsp->LEN=4;
        break;
    case FVATTR_DEVICEID:
		memcpy(fvRsp->PARAMS,(void*)&s_deviceID,4);
        fvRsp->LEN=4;
        break;
    case FVATTR_ADDR:
        memset(fvRsp->PARAMS,0,4);;
        fvRsp->LEN=4;
        break;
    case FVATTR_VERSION:
        fvRsp->PARAMS[0]=(UInt8)(DEV_VER>>8);	//版本号
        fvRsp->PARAMS[1]=(UInt8)DEV_VER;
        fvRsp->LEN=2;
        break;
    case FVATTR_STYLE:
        fvRsp->PARAMS[0]=(UInt8)(DEV_TYPE>>8);	//设备类型号
        fvRsp->PARAMS[1]=(UInt8)DEV_TYPE;	
        fvRsp->LEN=2;
        break;
	case FVATTR_CPUID:
		hal_CPU_getId(fvRsp->PARAMS);
        fvRsp->LEN=12;
        break;	
    case FVATTR_INIT_PARAMS:
        {
            int i;
            memcpy(fvRsp->PARAMS,(void*)(ROM_ADDRESS_START+ROM_PAGESIZE*ROM_CONFIG_PAGE),8);
            for(i=0;i<8;i++){
                fvRsp->PARAMS[i]=~fvRsp->PARAMS[i];
            }
            fvRsp->LEN=8;
        }
        break;
     case FVATTR_TIME:
        drv_CommonFunc_getTime(fvCmd,fvRsp);
        break;
     default:
        drv_CommFunc_setError(fvRsp,ERRCODE_UNSUPPORT);
        return;
    }
}

void drv_CommonFunc_setAttr(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    if(fvCmd->LEN<4){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    switch(fvCmd->PARAMS[0]){
    case FVATTR_TIME:
        drv_CommonFunc_setTime(fvCmd,fvRsp);
        break;
    default:
        drv_CommFunc_setError(fvRsp,ERRCODE_UNSUPPORT);
        return;
    }
}

void drv_CommonFunc_beep(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    
    // FV_COMMAND_PWM* pPWMCmd=(FV_COMMAND_PWM*)fvCmd->PARAMS;
   	// if(fvCmd->LEN<sizeof(FV_COMMAND_PWM)){
    //     drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
    //     return;
    // }
    drv_CommFunc_setError(fvRsp,ERRCODE_UNSUPPORT);
    // if(pPWMCmd->PWMSlot==0){
    //     if(pPWMCmd->PWMMode==0){
    //         fns_Beep_warn(pPWMCmd->PWMFreq,pPWMCmd->PWMCount,pPWMCmd->PWMOn,pPWMCmd->PWMOff);
    //     }else if(pPWMCmd->PWMMode==1){
    //         fns_Beep_poweron(pPWMCmd->PWMCount);
    //     }else if(pPWMCmd->PWMFreq==2){
    //         fns_Beep_ok(pPWMCmd->PWMCount);
    //     }else {
    //         fns_Beep_stop();
    //     }
    // } 
    
}
// 灯号(2B)+ 重复次数(2B)+循环1时间(int)(4B)+ 循环2时间(int)(4B)
void drv_CommonFunc_light(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    UInt8 ledNo;
    UInt16 repeat;
    int phase1;
    int phase2;
    ledNo=fvCmd->PARAMS[0];
   	if((fvCmd->LEN<12)||(ledNo>(LED_COUNT-1))){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }

    memcpy(&repeat,fvCmd->PARAMS+2,2);
    memcpy(&phase1,fvCmd->PARAMS+4,4);
    memcpy(&phase1,fvCmd->PARAMS+8,4);
    drv_Led_start(ledNo,repeat,phase1,phase2);
}


//在程序区写入出厂参数
//格式化参数: 工作模式(2B)+代理商编号(2B)+保留(4B)
HRESULT hal_ROM_init_(UInt8* params){
    UInt32 dataVal;
    
    //擦除
    if(FLASH_ErasePage(ROM_ADDRESS_START+ROM_CONFIG_PAGE*ROM_PAGESIZE)!=FLASH_COMPLETE){
        return ERRCODE_DEVICE_HAL;
    }
    //写入前4B
    memcpy(&dataVal,params,4);
    dataVal=~dataVal;
    if(FLASH_ProgramWord(ROM_ADDRESS_START+ROM_CONFIG_PAGE*ROM_PAGESIZE+0, dataVal)!=FLASH_COMPLETE ){
        return ERRCODE_DEVICE_HAL;
    }
    //写入后4B
    memcpy(&dataVal,params+4,4);
    dataVal=~dataVal;
    if(FLASH_COMPLETE!=FLASH_ProgramWord(ROM_ADDRESS_START+ROM_CONFIG_PAGE*ROM_PAGESIZE+4, dataVal) ){
        return ERRCODE_DEVICE_HAL;
    }

    return S_OK;
}
HRESULT hal_ROM_init(UInt8* params){
    HRESULT hr;
    FLASH_Unlock();
    if(FLASH_WaitForLastOperation(1000)!=FLASH_COMPLETE ){
        return ERRCODE_DEVICE_HAL;
    }
    hr=hal_ROM_init_(params);
    FLASH_Lock();
    return hr;
}
//格式化标志(8B)+格式化参数(8B)
void drv_CommonFunc_formatDevice(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    // HRESULT hr;
    // int i;
    if( fvCmd->LEN<16 ){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }

    // if(memcmp(S_FORMAT_FONT_TAG,fvCmd->PARAMS,8)==0){   //仅格式化字库
    //     hal_Flash_eraseSector(FLASH_FLAG_SECTOR);
    //     hal_Flash_setFlag(FLASH_FONT_PAGE,FLASH_FLAG_FONTNORMAL);
    //     hal_Flash_setFlag(FLASH_DATA_PAGE,FLASH_FLAG_DATANORMAL);
    //     //请求系统复位
    //     drv_CPU_requestReset(500);
    //     return;
    // }else if(memcmp(S_FORMAT_DATA_TAG,fvCmd->PARAMS,8)==0){    //仅格式化数据
    //     hal_Flash_eraseSector(FLASH_FLAG_SECTOR);
    //     hal_Flash_setFlag(FLASH_DATA_PAGE,FLASH_FLAG_FORMAT);
    //     hal_Flash_setFlag(FLASH_FONT_PAGE,FLASH_FLAG_FONTNORMAL);
    // }else if(memcmp(S_FORMAT_ALL_TAG,fvCmd->PARAMS,8)==0){
    //     hal_Flash_eraseSector(FLASH_FLAG_SECTOR);
    //     hal_Flash_setFlag(FLASH_DATA_PAGE,FLASH_FLAG_FORMAT);
    //     hal_Flash_setFlag(FLASH_FONT_PAGE,FLASH_FLAG_FORMAT);
    // }else{
    //     drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
    //     return;
    // }
    
    // hr=hal_ROM_init(fvCmd->PARAMS+8);
    // if(hr!=S_OK){
    //     drv_CommFunc_setError(fvRsp,hr);
    //     return;    
    // }
    // memcpy(fvRsp->PARAMS,(void*)(ROM_ADDRESS_START+ROM_CONFIG_PAGE*ROM_PAGESIZE),8);
    // for( i=0;i<8;i++){
    //     fvRsp->PARAMS[i]=~fvRsp->PARAMS[i];
    // }
    // fvRsp->LEN=8;
    
    //请求系统复位
    drv_CPU_requestReset(500);
}


void drv_CommonFunc_readDevice(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    FV_COMMAND_READ* pCmdRead=(FV_COMMAND_READ*)fvCmd->PARAMS; //要与cmd格式对应起来，cmd.params的参数至少要8个字节
    if((fvCmd->LEN<8)||(pCmdRead->Addr>1024*32)){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }

    if(drv_Flash_read(pCmdRead->Addr,fvRsp->PARAMS,pCmdRead->DataLen)){
        fvRsp->LEN=pCmdRead->DataLen;
    }else{
        drv_CommFunc_setError(fvRsp,ERRCODE_DEVICE_HAL);
    }
}

//写区域控制由上层SDK控制
void drv_CommonFunc_writeDevice(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    FV_COMMAND_WRITE* pCmdWrite=(FV_COMMAND_WRITE*)fvCmd->PARAMS;
    if((fvCmd->LEN<8)||(pCmdWrite->Addr>1024*32)){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
//    if(drv_Race_isSystemReady()){ //系统设置完成以后就不再允许直接写FLASH
//        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_STATE);
//        return;
//    }

    if(pCmdWrite->Addr<1024*32){
        if(pCmdWrite->DataLen==0){
            // drv_File_flush(&s_fontFile);
        }else{
            if(!drv_Flash_write(pCmdWrite->Addr,pCmdWrite->DataBuff,pCmdWrite->DataLen)){
               drv_CommFunc_setError(fvRsp,ERRCODE_DEVICE_HAL);
            }
        }
    }
}

//UInt32 m_io_buffer[8];
//void drv_CommonFunc_readIO(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
//    UInt32 nPort;

//    if(fvCmd->LEN<4){
//        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
//        return;
//    }

//    nPort=PtrToUInt32(fvCmd->PARAMS,true);
//    if(nPort<8){
//        memcpy(fvRsp->PARAMS,&m_io_buffer[nPort],4);
//    }else{
//        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);

//    }
//}
//void drv_CommonFunc_writeIO(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
//    UInt32 nPort;
//    nPort=PtrToUInt32(fvCmd->PARAMS,true);
//    if(nPort<8){
//        memcpy(&m_io_buffer[nPort],fvCmd->PARAMS+4,4);
//    }else{
//        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
//    }
//}
extern bool _raceVerify;
extern volatile MONITOR s_monitor;
void drv_CommonFunc_setTime(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    Int64 newTime;
    
   //参数检查
   if(fvCmd->LEN<(8+4)){
       drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
       return;
   }
   
    if( (ROM_WORKMODE==0) || memEquals(s_systemId,0,4) ){
        //尚未初始化,忽略权限检查
    }else{
        //权限检查
        if( (!_commVerify) ){
           drv_CommFunc_setError(fvRsp,ERRCODE_SECRITY_PERMIT);
           return;
        }
    } 

   //写入时钟
   memcpy(&newTime,fvCmd->PARAMS+4,8);
   drv_Time_set(newTime); 
}

void drv_CommonFunc_getTime(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    Int64 newTime=fns_Time_get();
    memcpy(fvRsp->PARAMS, &newTime,8);
    fvRsp->LEN=8;
}

void drv_CommonFunc_reBootDevice(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
	drv_CPU_requestReset(500);	//请求系统复位
}

/////////////////////////////////////安全系统////////////////////////////////////////////
//输入:系统ID(4B)+系统密码(8B)
void drv_SecrityFunc_GetAttr(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    if(fvCmd->LEN<4){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    switch(fvCmd->PARAMS[0]){
    case FVATTR_SYSTEMID:
        memcpy(fvRsp->PARAMS,s_systemId,4);
        fvRsp->LEN=4;
        break;
    case FVATTR_SYSMARK:
        memcpy(fvRsp->PARAMS,&s_systemMark,4);
        fvRsp->LEN=4;
        break;
    case FVATTR_TIMEZONE:
        // drv_CommonFunc_getTimeZone(fvCmd,fvRsp);
        break;
    }
}
void drv_SecrityFunc_SetAttr(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    if(fvCmd->LEN<4){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    switch(fvCmd->PARAMS[0]){
    case FVATTR_TIMEZONE:
        // drv_CommonFunc_setTimeZone(fvCmd,fvRsp);
        break;
    default:
        drv_CommFunc_setError(fvRsp,ERRCODE_UNSUPPORT);
    return;
    }
}
//代理商编号(2B)+学校编号(2B)+systemMark(4B)+密码(8B)
void fns_SecrityFunc_init(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    Int64 nowUTC;
    UInt32 systemId,systemMark;
    UInt64 systemKey;
    
    //参数检查
    if(fvCmd->LEN<16){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    
    if( fvCmd->PARAMS[0]!=ROM_PROXYID[0] || fvCmd->PARAMS[1]!=ROM_PROXYID[1] ){//@@1代理商编号什么时候设置的
        //代理商编号不匹配
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }
    
    nowUTC=fns_Time_get();
    // if(nowUTC<=UTC_MS_2000){
    //     //时钟异常
    //     drv_CommFunc_setError(fvRsp,ERRCODE_CMD_STATE);
    //     return;
    // }
    memcpy(&systemId,fvCmd->PARAMS,4);
    memcpy(&systemMark,fvCmd->PARAMS+4,4);
    memcpy(&systemKey,fvCmd->PARAMS+8,8);
    if( systemMark==0 ){ //默认用当前时间作为系统标记
        // systemMark=utcToSeconds(nowUTC,UTC_S_2000);
        systemMark=utcToSeconds(nowUTC,0);//@@1
    }
    
    //执行系统初始化
	fns_Flash_setSystemData(systemId,systemMark,systemKey);
    
    //返回systemMark
    memcpy(fvRsp->PARAMS,&systemMark,4);
    fvRsp->LEN=4;
    
    //设备重启
    drv_CPU_requestReset(500);
}

void fns_SecrityFunc_getSystemId(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    UInt8 systemId[4];
    fns_Flash_getSystemData(systemId,NULL,NULL);
    memcpy(fvRsp->PARAMS,&systemId,4);
    fvRsp->LEN=4;
}

void fns_SecrityFunc_verify(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp){
    UInt8 systemId[4];
    
    //参数检查
    if(fvCmd->LEN<12){
        drv_CommFunc_setError(fvRsp,ERRCODE_CMD_PARAMS);
        return;
    }

    memcpy(systemId,fvCmd->PARAMS,4);
    if( !drv_Main_isSameSystem(systemId,fvCmd->PARAMS+4) ){
        drv_CommFunc_setError(fvRsp,ERRCODE_SECRITY_VERIFY);
        return;
    }      
    _commVerify=true;
}
