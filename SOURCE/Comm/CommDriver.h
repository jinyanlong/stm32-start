//ͨѶ֡�ṹ
#ifndef COMMPACKET_H
#define COMMPACKET_H

#include "MessageLoop.h"
#include "halUart.h"

#define CHAR_STB 0x1A
#define CHAR_ETB 0x17
#define CHAR_ESC 0x1B

#define UART_CHAR_INTERVAL   500    //�ַ����ms

#define UART_PACKETSTATE_IDLE         0        //׼������

#define UART_PACKETSTATE_START        0x10   //����Э���־
#define UART_PACKETSTATE_DATA         0x11     //���ڽ������ݰ�
#define UART_PACKETSTATE_ESC          0x12     //���ڽ���ת���ַ�

#define UART_PACKETSTATE_ETB1         0x20
#define UART_PACKETSTATE_ETB2         0x21

#define UART_PACKETSTATE_FINISHED     0x80     //������ɵȴ�����
#define UART_PACKETSTATE_ERROR        0x40     //ʧ��

#define UART_PACKETSTATE_OK           (0|UART_PACKETSTATE_FINISHED)     //������ɵȴ�����
#define UART_PACKETSTATE_SLAVER       (1|UART_PACKETSTATE_FINISHED)     //����ģʽ
#define UART_PACKETSTATE_MASTER       (2|UART_PACKETSTATE_FINISHED)     //�ӻ�ģʽ

#define UART_PACKETSTATE_ERR_ESC      (1|UART_PACKETSTATE_FINISHED|UART_PACKETSTATE_ERROR)
#define UART_PACKETSTATE_ERR_LEN      (2|UART_PACKETSTATE_FINISHED|UART_PACKETSTATE_ERROR)
#define UART_PACKETSTATE_ERR_ADDR     (3|UART_PACKETSTATE_FINISHED|UART_PACKETSTATE_ERROR)
#define UART_PACKETSTATE_ERR_CRC      (4|UART_PACKETSTATE_FINISHED|UART_PACKETSTATE_ERROR)
#define UART_PACKETSTATE_ERR_PROT     (5|UART_PACKETSTATE_FINISHED|UART_PACKETSTATE_ERROR)

#define UART_BUFFER_MAXSIZE   256    //���������ߴ�

#define FVTYPE_COMMAND 0x00
#define FVTYPE_NOTIFY  0x01
#define FVTYPE_ACK     0x02
#define FVTYPE_NAK     0x03

#define FVCLA_GETTYPE(cla)  ((cla)>>6)
#define FVCLA_SETTYPE(cla,type)  cla=(cla&0x3F)|(((type)&0x03)<<6)

#define FVCLA_GETID(cla)     ((cla)&0x3F)

//����ȷ��4�ֽڶ���
typedef struct _FV_PACKET{    
    UInt8  CLA;                         //��������
    UInt8  INS;                         //ָ����
    UInt16 LEN;                         //��������
	UInt8  PARAMS[UART_BUFFER_MAXSIZE-1-8-4-1];       //��������
} FV_PACKET;
typedef FV_PACKET FV_COMMAND;
typedef FV_PACKET FV_RESPONSE;
typedef struct _UART_PACKET_STATE{
    UInt8   state;
    UInt8   eof;
    UInt8   protocol;         //Э��ģʽ
    UInt8*  stream;
    
    UInt8   last;     //���һ���յ�������
    UInt8   crc8;      //CRC8���㻺��
} UART_PACKET_PARSER;

typedef struct _UartPacketHandler{
    USART_TypeDef * pUart; 
#ifdef __RS485
    USART_TypeDef * pUartRS485;
#endif
    UInt8   stream[UART_BUFFER_MAXSIZE];  //���ڻ�����
    UInt16  eof;     //��������һ����λ
    
    //bool slaveMode;         //ͨѶ�Ƿ��ڴ�ģʽ
    //UInt32 masterAddr;      //������ַ
    //UInt32 slaverAddr;      //�ӻ���ַ
    


    UInt8 rspINS; //�ȴ�NRF�ظ���ins

    bool lazyMode;
    volatile UInt32 rspTimout;
    FV_COMMAND cmdPacket;   //ָ��
    FV_RESPONSE rspPacket;  //Ӧ��

  

}UartPacketHandler;


typedef struct _NRFFV_PACKET{
    UInt8  RETAIN;
    UInt8  TOGGLE;
    UInt8  RSSI;
    UInt8  LEN;                         
    UInt8  CHANEL;   
    UInt8  CLA;                         
    UInt8  INS;  
	UInt8  PARAMS[UART_BUFFER_MAXSIZE-2];       //��������
} NRFFV_PACKET;

typedef struct _NrfUartPacketHandler{
    USART_TypeDef * pUart; 

    UInt8   stream[UART_BUFFER_MAXSIZE];  //���ڻ�����
    UInt16  eof;     //��������һ����λ
        
    NRFFV_PACKET cmdPacket;   //�ӿ�nrfģ������
    NRFFV_PACKET rspPacket;  

}NrfUartPacketHandler;



static __INLINE void drv_CommFunc_setError(FV_RESPONSE* rsp,UInt16 error){
    FVCLA_SETTYPE(rsp->CLA,FVTYPE_NAK);
    rsp->LEN=2;
    
    rsp->PARAMS[0]= (UInt8)(error>>0);
    rsp->PARAMS[1]= (UInt8)(error>>8);
}


void drv_Comm_init(void);

void drv_Comm_startRx(UartPacketHandler* pUartHandler);
void drv_Comm_onCommand(FV_PACKET* cmdPacket,FV_PACKET* rspPacket);
void drv_Comm_onNotify(FV_COMMAND* cmdPacket);
void drv_Comm_sendPacket(USART_TypeDef * pUart,UInt8* head,FV_PACKET* pFVPacket);
void drv_Comm_parseSTBPacket(UART_PACKET_PARSER* parser,UInt8* buffer,UInt16 len);
void drv_Comm_parseETBPacket(UART_PACKET_PARSER* parser,UInt8* buffer,UInt16 len);
void drv_Comm_sendByte(USART_TypeDef * pUart,UInt8 ch);
void drv_Comm_sendBytes(USART_TypeDef * pUart,void* pDataBuff ,int nCount);

#endif
