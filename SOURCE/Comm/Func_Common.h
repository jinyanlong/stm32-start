#ifndef FUNCCOMMON_H
#define FUNCCOMMON_H

#include "CommDriver.h"

#define FVATTR_STATE        0
#define FVATTR_DEVICEID     1
#define FVATTR_ADDR         2
#define FVATTR_VERSION      3
#define FVATTR_STYLE        4
#define FVATTR_CPUID    	5
#define FVATTR_INIT_PARAMS  6
#define FVATTR_TIME         7


typedef void (*FVCMD_FUNCLIST[])(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
typedef void (*FVCMD_FUNCTION)(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
typedef void (*FVNOTIFY_FUNCLIST[])(FV_COMMAND* fvCmd);




#define FVCLA_DEVICE   0     //�豸ָ��
#define FVCLA_SYSTEM  1      //����ָ��

#define FVATTR_SYSTEMID     0
#define FVATTR_SYSMARK      1
#define FVATTR_TIMEZONE     2

//�豸���ָ��
#define FVCLA_SCHOOL    ((UInt8)(DEV_TYPE))
#define FVCLA_SCHOOL_CMD    (FVCLA_SCHOOL&0x3F)|(((FVTYPE_COMMAND)&0x03)<<6)
#define FVCLA_SCHOOL_MSG    (FVCLA_SCHOOL&0x3F)|(((FVTYPE_NOTIFY)&0x03)<<6)

#define FVAPPLY_DEVICENAME   0
#define FVAPPLY_NETIP        1
#define FVAPPLY_NETPATH      2
#define FVAPPLY_APN          3
#define FVAPPLY_WEBPING      4
#define FVAPPLY_TAGPING      5
#define FVAPPLY_TAGOFFTIME   6
#define FVAPPLY_UARTPING     7      //UART ping���
#define FVAPPLY_UARTOVERTIME 8      //����ͨѶģʽ��ʱ���





typedef struct _FV_COMMAND_PWM{
    UInt8   PWMSlot;
    UInt8   PWMMode;
    UInt16  PWMFreq;
    UInt16  PWMCount;
	UInt16  PWMOn;
	UInt16  PWMOff;
} FV_COMMAND_PWM;

typedef struct _FV_COMMAND_READ{
    UInt32  Addr;
    UInt32  DataLen;
} FV_COMMAND_READ;

typedef struct _FV_COMMAND_WRITE{
    UInt32  Addr;
    UInt32  DataLen;
    UInt8   DataBuff[1];
} FV_COMMAND_WRITE;


void drv_CommFunc_call(FVCMD_FUNCLIST pFunc,int index,int nMaxCount,FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommFunc_callNotify(FVNOTIFY_FUNCLIST pFunc,int index,int nMaxCount,FV_COMMAND* fvCmd);
void drv_CommFunc_reportError(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp,UInt16 nErrType);
void drv_CommFunc_reserved(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);

void drv_CommonFunc_getAttr(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommonFunc_setAttr(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommonFunc_beep(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommonFunc_formatDevice(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommonFunc_readDevice(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommonFunc_writeDevice(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
//void drv_CommonFunc_readIO(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
//void drv_CommonFunc_writeIO(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommonFunc_getTime(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommonFunc_setTime(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommonFunc_reBootDevice(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommonFunc_setTimeZone(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_CommonFunc_getTimeZone(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);


void drv_SecrityFunc_GetAttr(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void drv_SecrityFunc_SetAttr(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void fns_SecrityFunc_init(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void fns_SecrityFunc_getSystemId(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void fns_SecrityFunc_auth1(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void fns_SecrityFunc_auth2(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void fns_SecrityFunc_verify(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);
void fns_SecrityFunc_setValidate(FV_COMMAND* fvCmd,FV_RESPONSE* fvRsp);


//////////////////////���ô������////////////////////////
#define ERRTYPE_COMM          0x0100
#define ERRCODE_PACKET_CRC    (ERRTYPE_COMM+0x01)     //CRCУ�����
#define ERRCODE_PACKET_FORMAT (ERRTYPE_COMM+0x02)     //���ݰ���ʽ����
#define ERRCODE_PACKET_PARAMS (ERRTYPE_COMM+0x03)     //���ݰ���������
#define ERRCODE_PACKET_LEN    (ERRTYPE_COMM+0x04)     //���ݰ����ȴ���
#define ERRCODE_PACKET_STATE  (ERRTYPE_COMM+0x05)     //���ݰ�״̬����

#define ERRCODE_UNSUPPORT      (ERRTYPE_COMM+0x08)     //��֧�ֵ�ָ��
#define ERRCODE_CMD_PARAMS     (ERRTYPE_COMM+0x09)     //�����ָ�����
#define ERRCODE_CMD_STATE      (ERRTYPE_COMM+0x0A)    //δ���������Ĺ���״̬
#define ERRCODE_SECRITY_PERMIT (ERRTYPE_COMM+0x0B)    //Ȩ�޲���
#define ERRCODE_SECRITY_SYSID  (ERRTYPE_COMM+0x0C)    //ϵͳID��ƥ��
#define ERRCODE_SECRITY_VERIFY (ERRTYPE_COMM+0x0D)    //У��ʧ��
#define ERRCODE_CMD_OVER       (ERRTYPE_COMM+0x0E)     //�������
#define ERRCODE_CMD_OVERTIME       (ERRTYPE_COMM+0x10)
//#define ERRTYPE_XXX	      0x0200    //������PC��ʹ��

#define ERRTYPE_DEVICE	      0x0300    //��������
#define ERRCODE_DEVICE_HAL	  (ERRTYPE_DEVICE+0x00)

#define ERRTYPE_LF          0x0400
#define ERRCODE_LF_BUSY    (ERRTYPE_LF+0x01)     //lf���ڷ�������


#define PCCOM_RECV_TIMOUT 200

#define PCCOMM_STATE_NOACK 0X00
#define PCCOMM_STATE_ACK 0XFF //�����ظ�

#define PCCOMM_STATE_CALI 0X01  //pcComm״̬
#define PCCOMM_STATE_GETATTR 0X02
#define PCCOMM_STATE_INITDEV 0X03
#define PCCOMM_STATE_READPARA 0X04


#define LF_NRFACK 0XA1  //��Ƶ����
    #define LF_CALI 0X80  //����У׼
    #define LF_GETATTR 0X81  //��ȡ��ǩ����
        #define LF_GETATTR_TAGSTATE 0X8100
        #define LF_GETATTR_TAGNUM 0X8101
        #define LF_GETATTR_VERSION 0X8103
        #define LF_GETATTR_VERTYPE 0X8104
        #define LF_GETATTR_CPUID 0X8105
        #define LF_GETATTR_REGNOWTIME 0X8106
        #define LF_GETATTR_SYSTEMID 0X8107
        #define LF_GETATTR_PINGINTERAL 0X8108
        #define LF_GETATTR_OFFLINEINTERAL 0X8109
        #define LF_GETATTR_SENDCOUNT 0X810A
        #define LF_GETATTR_CONFINFO 0X810B
    #define LF_CHANGE_REVSTATE 0X82  //ǿ�Ʊ�ǩ����/�˳�2.4G����״̬
#define NRF_MCU2TAG 0XA2    //��Ƶ����
#define NRF_TAG2MCU 0XA3    //��Ƶ����

//pc����ͨ��2.4G����
#define PCCMD_INITDEV 0X01  //��ʼ����ǩ
#define PCCMD_READPARA 0X02 //��ȡ����

void fns_SetPcComm_State(UInt8 data);
UInt8 fns_GetPcComm_State(void);

#endif
