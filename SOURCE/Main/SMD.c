#ifdef __NET

#include "includes.h"
#include "MessageLoop.h"

#include "Beep.h"
#include "RealTimer.h"
#include "DesCrypt.h"
#include "RaceData.h"
#include "RaceTask.h"
#include "RaceFunc.h"
#include "SMD.h"
#include "GPS.h"
#include "GPRS.h"
#include "font.h"
#include "md5.h"
#include "tagList.h"
extern UInt8 s_systemId[4];
extern UInt32 s_systemMark;
extern UInt8 s_systemKey[8];
extern UInt32 s_deviceID;
extern HTTPSTRUCT s_http;

//token-yz-school: {systemId}+";"+{utcʱ��}+";"+{ǩ��(HEX�ַ���ʽ)};
MD5_CTX md5_ctx;

void drv_HTTPHead_pushToken(void){
    char buffer[33];
    drv_HTTPHead_pushString("token-yz:school-v1;");
   
    //systemId
    MD5Init(&md5_ctx); 
    BinToHex(s_systemId, buffer,4);
    MD5Update(&md5_ctx,(UInt8*)buffer,8);
    drv_HTTPHead_pushString(buffer);
    drv_HTTPHead_pushChar(';');
    
    //deviceId
    sprintf(buffer,"%08X",s_deviceID);
    MD5Update(&md5_ctx,(UInt8*)buffer,8);
    drv_HTTPHead_pushString(buffer);
    drv_HTTPHead_pushChar(';');
    
    //time
    sprintf(buffer,"%lld",fns_Time_get());
    MD5Update(&md5_ctx,(UInt8*)buffer,strlen(buffer));
    drv_HTTPHead_pushString(buffer);
    drv_HTTPHead_pushChar(';');
    
    //����body�е�MD5
    MD5Update(&md5_ctx,s_systemKey,8);
    MD5Final(&md5_ctx,(UInt8*)buffer);
    drv_HTTPHead_pushHex(buffer,16);

    drv_HTTPHead_pushLine();
}
void drv_HTTPHead_pushLength(void){
     drv_HTTPHead_push("Content-Length: %d\r\n",s_http.bodySize);
}

//����: �ͺ�(ESG01),�豸���
void drv_SMD_sendRegister(void){
	//���HTTP������
    drv_HTTP_clear();
    
    
    //�ȷ�����    
    drv_HTTPBody_pushString("{\"params\":{\"deviceType\":\"ESG01\",\"deviceId\":");
    drv_HTTPBody_push("\"%08X\"",s_deviceID);
    drv_HTTPBody_pushString("},\"method\":\"deviceAuth\"}");  //externAuth�ⲿ��Ȩ��֤
    
    
    //POST
    drv_HTTPHead_pushString("POST " YZ_DEVICE_PATH " HTTP/1.1\r\n");
  
    //HOST
    drv_HTTPHead_pushString("Host:" YZ_HOST_IP "\r\n");

    //Content-Type
    drv_HTTPHead_pushString("Content-Type: application/json-rpc;charset=UTF-8\r\n");
    
    //����
    drv_HTTPHead_pushLength();

    //���ݿ�ʼ
    drv_HTTPHead_pushLine();

    msg_Gsm_sendHttpYz(&s_http);
}

void drv_SMD_sendPing(MONITOR* monitor){    
    ASCCHARS ascText;

	//���HTTP������
    drv_HTTP_clear();
    
    //����    
    drv_HTTPBody_pushString("{\"params\":[");
    drv_HTTPBody_push("%d",monitor->uploadSOF);
    drv_HTTPBody_push(",%d",monitor->totalEOF-monitor->uploadSOF);
    drv_HTTPBody_pushString("],\"method\":\"ping\",\"id\":2}");

    //POST
    drv_HTTPHead_pushString("POST");
    drv_HTTPHead_pushChar(' ');
    drv_Data_getAString(ASTRING_TYPE_DIR,&ascText);
    drv_HTTPHead_pushAString(&ascText);
    drv_HTTPHead_pushChar(' ');
    drv_HTTPHead_pushString("HTTP/1.1");
    drv_HTTPHead_pushLine();
  
    //HOST
    drv_HTTPHead_pushString("Host: ");
    drv_Data_getAString(ASTRING_TYPE_IP,&ascText);
    drv_HTTPHead_pushAString(&ascText);
    drv_HTTPHead_pushLine();

    //Content-Type    
    drv_HTTPHead_pushString("Content-Type: application/json-rpc;charset=UTF-8\r\n");
    drv_HTTPHead_pushToken();
    //����Content-Length
    drv_HTTPHead_pushLength();
   
    //���ݿ�ʼ
    drv_HTTPHead_pushLine();

    msg_Gsm_sendHttp(&s_http);        
}


void drv_SMD_sendRecords(UInt32 sof, UInt32 count){ //���ӷ���һ����ѹ������־λ  (tagRecord.flag&0x80)   0/1
    ASCCHARS ascText;
    TAGRECORD tagRecord;
    char buffer[32];
    UInt32 i;

	//���HTTP������
    drv_HTTP_clear();
    
    //����    
    drv_HTTPBody_pushString("{\"params\":[");
    drv_HTTPBody_pushChar('[');
    
    for(i=0;i<count;i++){
        if(i>0) drv_HTTPBody_pushChar(',');
        
        drv_HTTPBody_pushChar('"');
        drv_Record_get(sof+i,&tagRecord);
        
        drv_HTTPBody_push("%d",tagRecord.flag);                       
        drv_HTTPBody_pushChar(',');        
        
        drv_Coin_toString(tagRecord.coinId,buffer);
        drv_HTTPBody_pushString(buffer);
        drv_HTTPBody_pushChar(',');
        
        drv_Tag_toString(tagRecord.tagId,buffer);
        drv_HTTPBody_pushString(buffer);
        drv_HTTPBody_push(",%d",tagRecord.steper);
                
        drv_HTTPBody_push(",%lld",TICK2UTC(tagRecord.baseTime,tagRecord.raiseTick));
        drv_HTTPBody_push(",%lld",TICK2UTC(tagRecord.baseTime,tagRecord.saveTick));
        
        //׷��2���ֶ�,systemMark,recordId
        {
            DATETIME dt;
            SecondsToTime(s_systemMark,&dt);    
            sprintf(buffer,",%02d%02d%02d%02d%02d%02d",dt.Year,dt.Month,dt.Day,dt.Hour,dt.Minute,dt.Second);
            drv_HTTPBody_pushString(buffer);
            
            sprintf(buffer,",%d",sof+i);
            drv_HTTPBody_pushString(buffer);
        }
        drv_HTTPBody_pushChar('"');        
        
    }
    
    drv_HTTPBody_pushChar(']');
    drv_HTTPBody_pushString("],\"method\":\"sendRecords\",\"id\":3}");

    //POST
    drv_HTTPHead_pushString("POST");
    drv_HTTPHead_pushChar(' ');
    drv_Data_getAString(ASTRING_TYPE_DIR,&ascText);
    drv_HTTPHead_pushAString(&ascText);
    drv_HTTPHead_pushChar(' ');
    drv_HTTPHead_pushString("HTTP/1.1");
    drv_HTTPHead_pushLine();
  
    //HOST
    drv_HTTPHead_pushString("Host: ");
    drv_Data_getAString(ASTRING_TYPE_IP,&ascText);
    drv_HTTPHead_pushAString(&ascText);
    drv_HTTPHead_pushLine();

    //Content-Type    
    drv_HTTPHead_pushString("Content-Type: application/json-rpc;charset=UTF-8\r\n");
    drv_HTTPHead_pushToken();
    //����Content-Length
    drv_HTTPHead_pushLength();
    
    //���ݿ�ʼ
    drv_HTTPHead_pushLine();

    msg_Gsm_sendHttp(&s_http);    
    
}

#endif //__NET
