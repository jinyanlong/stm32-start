#include "includes.h"
#include "halCPU.h"
#include "SFile.h"
#include "RaceData.h"
#include "RealTimer.h"
//#include "halGPRS.h"
/////////////////////////////////////////////////////////////////////
//该文件是一些单纯的FLASH数据操作函数,不涉及RaceTask运行状态
//业务逻辑相关的函数在AppFunc.c 或者RaceTask中实现
/////////////////////////////////////////////////////////////////////
extern const SFILE s_dataFile;

void drv_Data_flush(void){
    drv_File_flush(&s_dataFile);
}

void drv_Data_setSystem(UInt8* systemId,UInt32 systemMark,UInt8* pKey){    
    drv_File_write(&s_dataFile,FILEADDR_SYSID,systemId,4);
    drv_File_write(&s_dataFile,FILEADDR_SYSMAC,&systemMark,4);
    drv_File_write(&s_dataFile,FILEADDR_SYSKEY,pKey,8);
}


void drv_Data_getSystem(UInt8* systemId,UInt32* systemMark,UInt8* pKey){
    if(systemId){
        drv_File_read(&s_dataFile,FILEADDR_SYSID,systemId,4);
    }
    
    if(pKey){
        drv_File_read(&s_dataFile,FILEADDR_SYSKEY,pKey,8);
    }

    if(systemMark){
        drv_File_read(&s_dataFile,FILEADDR_SYSMAC,systemMark,4);
    }
}


UInt32 drv_Data_getTotalSOF(void){
    UInt32 value;
    drv_File_read(&s_dataFile,FILEADDR_TOTALSOF,&value,4);
    return value;
}
UInt32 drv_Data_getTotalEOF(void){
    UInt32 value;
    drv_File_read(&s_dataFile,FILEADDR_TOTALEOF,&value,4);
    return value;
}
UInt32 drv_Data_getUploadSOF(void){
    UInt32 value;
    drv_File_read(&s_dataFile,FILEADDR_WEB_UPLOADSOF,&value,4);
    return value;
}
UInt32 drv_Data_getUartUploadSOF(void){
    UInt32 value;
    drv_File_read(&s_dataFile,FILEADDR_UART_UPLOADSOF,&value,4);
    return value;
}

void drv_Data_setTotalSOF(UInt32 value){
    drv_File_write(&s_dataFile,FILEADDR_TOTALSOF,&value,4);

}
void drv_Data_setTotalEOF(UInt32 value){
    drv_File_write(&s_dataFile,FILEADDR_TOTALEOF,&value,4);

}
void drv_Data_setUploadSOF(UInt32 value){
    drv_File_write(&s_dataFile,FILEADDR_WEB_UPLOADSOF,&value,4);
}
void drv_Data_setUartUploadSOF(UInt32 value){
    drv_File_write(&s_dataFile,FILEADDR_UART_UPLOADSOF,&value,4);
}


void drv_Data_getAString(UInt8 nMode,AStringBuffer pURI){
    switch(nMode){
    case ASTRING_TYPE_IP:
        drv_File_read(&s_dataFile,FILEADDR_SERVERIP,pURI,64);
        if(pURI->length>63) pURI->length=63;      
        break;
    case ASTRING_TYPE_DIR:
        drv_File_read(&s_dataFile,FILEADDR_SERVERSERVICE,pURI,64);      
        if(pURI->length>63) pURI->length=63;      
        break;
    case ASTRING_TYPE_APN:
        drv_File_read(&s_dataFile,FILEADDR_SERVERMETHOD,pURI,64);      
        if(pURI->length>63) pURI->length=63;      
        break;
    }
}


UInt16 drv_Data_getWebPingInterval(void){
    UInt16 value;
    drv_File_read(&s_dataFile,FILEADDR_WEBPING_INTERVAL,&value,2);
    if(value<60) value=180;
    return value;
}

void drv_Data_setWebPingInterval(UInt16 value){
    drv_File_write(&s_dataFile,FILEADDR_WEBPING_INTERVAL,&value,2);
}


UInt16 drv_Data_getTagOfftime(void){
    UInt16 value;
    drv_File_read(&s_dataFile,FILEADDR_TAG_OFFTIME,&value,2);
    if(value<1) value=3;
    return value;
}

void drv_Data_setTagOfftime(UInt16 value){
    drv_File_write(&s_dataFile,FILEADDR_TAG_OFFTIME,&value,2);
}


UInt16 drv_Data_getTagPingInterval(void){
    UInt16 value;
    drv_File_read(&s_dataFile,FILEADDR_TAGPING_INTERVAL,&value,2);
    if(value<10) value=60;
    return value;
}

void drv_Data_setTagPingInterval(UInt16 value){
    drv_File_write(&s_dataFile,FILEADDR_TAGPING_INTERVAL,&value,2);
}


void drv_Data_setTimeZone(Int16 zone){
    drv_File_write(&s_dataFile,FILEADDR_TIMEZONE,&zone,2);
}
Int16 drv_Data_getTimeZone(void){
    Int16 nOffset;
    drv_File_read(&s_dataFile,FILEADDR_TIMEZONE,&nOffset,2);
    if( (nOffset>12*60) || (nOffset<-12*60) ){
        return 0;
    }else{
        return nOffset;
    }
}

void drv_Data_setUartPingInterval(UInt16 value){
    drv_File_write(&s_dataFile,FILEADDR_UARTPING_INTERVAL,&value,2);
}

UInt16 drv_Data_getUartPingInterval(void){
    UInt16 value;
    drv_File_read(&s_dataFile,FILEADDR_UARTPING_INTERVAL,&value,2);
    if(value<3) value=15;
    return value;
}
void drv_Data_setUartOvertime(UInt16 value){
    drv_File_write(&s_dataFile,FILEADDR_UARTOVERTIME,&value,2);
}

UInt16 drv_Data_getUartOvertime(void){
    UInt16 value;
    drv_File_read(&s_dataFile,FILEADDR_UARTOVERTIME,&value,2);
    if(value<10) value=180;
    return value;
}

//pSiteName必须是UString类型
void drv_Data_setSiteName(const void* pSiteName){
    UInt16 nLen;
    UInt8 buff[64];
    
    memset(buff,0,64);
    if(pSiteName){
        memcpy(&nLen,pSiteName,2);
        if(nLen>30) nLen=30;     
        memcpy(buff,pSiteName,2*(nLen+1));
    }
    drv_File_write(&s_dataFile,FILEADDR_SITENAME,buff,64);
}
void drv_Data_getSiteName(UStringBuffer pSiteName){
    drv_File_read(&s_dataFile,FILEADDR_SITENAME,pSiteName,64);
    if(pSiteName->length>31) pSiteName->length=31;
}

void drv_Data_setAString(UInt8 nMode,const void* pAString){ //(fvCmd->PARAMS[0],(AString)(fvCmd->PARAMS+4))
    UInt8 nLen;
    UInt8 buff[64];
    
    memset(buff,0,64);
    if(pAString){
        memcpy(&nLen,pAString,1);
        if(nLen>62) nLen=62;
        memcpy(buff,pAString,nLen+1);
    }

    switch(nMode){
    case ASTRING_TYPE_IP:  
        drv_File_write(&s_dataFile,FILEADDR_SERVERIP,buff,64);      
        break;
    case ASTRING_TYPE_DIR:
        drv_File_write(&s_dataFile,FILEADDR_SERVERSERVICE,buff,64);      
        break;
    case ASTRING_TYPE_APN:
        drv_File_write(&s_dataFile,FILEADDR_SERVERMETHOD,buff,64);      
        break;
    }
}


void drv_Record_set(UInt32 recordIndex,const TAGRECORD* tagItem){
    drv_File_write(&s_dataFile,FILEADDR_TAGLIST+(recordIndex%TAG_RECORDS_CAPACITY)*TAG_RECORD_SIZE,tagItem,sizeof(TAGRECORD));
}

void drv_Record_get(UInt32 recordIndex,TAGRECORD* tagItem){
    drv_File_read(&s_dataFile,FILEADDR_TAGLIST+(recordIndex%TAG_RECORDS_CAPACITY)*TAG_RECORD_SIZE,tagItem,sizeof(TAGRECORD));
}



//////////////////////////////////////////////////////////////////////////////////////
//不要把EOF清成0,要保持原有值
void drv_Data_clearTags(void){    
    UInt32 eof=drv_Data_getTotalEOF();
    drv_Data_setTotalSOF(eof);
    drv_Data_setUploadSOF(eof);
    drv_Data_setUartUploadSOF(eof);    
}

//清除数据,但保留记录号
void drv_Data_clearBase(void){    
    drv_Data_clearTags();
    drv_Data_setTimeZone(480);
    drv_Data_setTagOfftime(3);
    drv_Data_setTagPingInterval(60);  //1分钟1次
    drv_Data_setWebPingInterval(180); //3分钟1次
    
    drv_Data_setAString(ASTRING_TYPE_IP,NULL);
    drv_Data_setAString(ASTRING_TYPE_DIR,NULL);
    drv_Data_setAString(ASTRING_TYPE_APN,NULL); 
    drv_Data_setSiteName(NULL);
}

//清空所有数据，恢复出厂设置
void drv_Data_reset(void){
    drv_Data_setTotalSOF(0);
    drv_Data_setTotalEOF(0);
    drv_Data_setUploadSOF(0);
    drv_Data_setUartUploadSOF(0); 
    drv_Data_setTimeZone(480);
    drv_Data_setTagOfftime(3);
    drv_Data_setTagPingInterval(60);  //1分钟1次
    drv_Data_setWebPingInterval(180); //3分钟1次
    
    drv_Data_setAString(ASTRING_TYPE_IP,NULL);
    drv_Data_setAString(ASTRING_TYPE_DIR,NULL);
    drv_Data_setAString(ASTRING_TYPE_APN,NULL); 
    drv_Data_setSiteName(NULL);
}

void drv_Data_clear(UInt8 nMode){
    if(nMode==0){
        drv_Data_clearBase();
        return;
    }
    
    if(nMode&0x01){
        drv_Data_clearTags();
    }

}

