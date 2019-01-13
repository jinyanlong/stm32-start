//通讯帧结构
#ifndef COMMPACKET_H
#define COMMPACKET_H

#include "MessageLoop.h"
#include "halUart.h"

#define CHAR_STB 0x1A
#define CHAR_ETB 0x17
#define CHAR_ESC 0x1B

#define UART_CHAR_INTERVAL   500    //字符间隔ms

#define UART_PACKETSTATE_IDLE         0        //准备接收

#define UART_PACKETSTATE_START        0x10   //接收协议标志
#define UART_PACKETSTATE_DATA         0x11     //正在接收数据包
#define UART_PACKETSTATE_ESC          0x12     //正在接收转义字符

#define UART_PACKETSTATE_ETB1         0x20
#define UART_PACKETSTATE_ETB2         0x21

#define UART_PACKETSTATE_FINISHED     0x80     //接收完成等待处理
#define UART_PACKETSTATE_ERROR        0x40     //失败

#define UART_PACKETSTATE_OK           (0|UART_PACKETSTATE_FINISHED)     //接收完成等待处理
#define UART_PACKETSTATE_SLAVER       (1|UART_PACKETSTATE_FINISHED)     //主从模式
#define UART_PACKETSTATE_MASTER       (2|UART_PACKETSTATE_FINISHED)     //从机模式

#define UART_PACKETSTATE_ERR_ESC      (1|UART_PACKETSTATE_FINISHED|UART_PACKETSTATE_ERROR)
#define UART_PACKETSTATE_ERR_LEN      (2|UART_PACKETSTATE_FINISHED|UART_PACKETSTATE_ERROR)
#define UART_PACKETSTATE_ERR_ADDR     (3|UART_PACKETSTATE_FINISHED|UART_PACKETSTATE_ERROR)
#define UART_PACKETSTATE_ERR_CRC      (4|UART_PACKETSTATE_FINISHED|UART_PACKETSTATE_ERROR)
#define UART_PACKETSTATE_ERR_PROT     (5|UART_PACKETSTATE_FINISHED|UART_PACKETSTATE_ERROR)

#define UART_BUFFER_MAXSIZE   256    //缓冲区最大尺寸

#define FVTYPE_COMMAND 0x00
#define FVTYPE_NOTIFY  0x01
#define FVTYPE_ACK     0x02
#define FVTYPE_NAK     0x03

#define FVCLA_GETTYPE(cla)  ((cla)>>6)
#define FVCLA_SETTYPE(cla,type)  cla=(cla&0x3F)|(((type)&0x03)<<6)

#define FVCLA_GETID(cla)     ((cla)&0x3F)

//必须确保4字节对齐
typedef struct _FV_PACKET{    
    UInt8  CLA;                         //参数长度
    UInt8  INS;                         //指令码
    UInt16 LEN;                         //参数长度
	UInt8  PARAMS[UART_BUFFER_MAXSIZE-1-8-4-1];       //输入数据
} FV_PACKET;
typedef FV_PACKET FV_COMMAND;
typedef FV_PACKET FV_RESPONSE;
typedef struct _UART_PACKET_STATE{
    UInt8   state;
    UInt8   eof;
    UInt8   protocol;         //协议模式
    UInt8*  stream;
    
    UInt8   last;     //最后一次收到的数据
    UInt8   crc8;      //CRC8计算缓冲
} UART_PACKET_PARSER;

typedef struct _UartPacketHandler{
    USART_TypeDef * pUart; 
#ifdef __RS485
    USART_TypeDef * pUartRS485;
#endif
    UInt8   stream[UART_BUFFER_MAXSIZE];  //串口缓冲区
    UInt16  eof;     //缓冲区下一个空位
    
    //bool slaveMode;         //通讯是否处在从模式
    //UInt32 masterAddr;      //主机地址
    //UInt32 slaverAddr;      //从机地址
    


    UInt8 rspINS; //等待NRF回复的ins

    bool lazyMode;
    volatile UInt32 rspTimout;
    FV_COMMAND cmdPacket;   //指令
    FV_RESPONSE rspPacket;  //应答

  

}UartPacketHandler;


typedef struct _NRFFV_PACKET{
    UInt8  RETAIN;
    UInt8  TOGGLE;
    UInt8  RSSI;
    UInt8  LEN;                         
    UInt8  CHANEL;   
    UInt8  CLA;                         
    UInt8  INS;  
	UInt8  PARAMS[UART_BUFFER_MAXSIZE-2];       //输入数据
} NRFFV_PACKET;

typedef struct _NrfUartPacketHandler{
    USART_TypeDef * pUart; 

    UInt8   stream[UART_BUFFER_MAXSIZE];  //串口缓冲区
    UInt16  eof;     //缓冲区下一个空位
        
    NRFFV_PACKET cmdPacket;   //接口nrf模块数据
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
