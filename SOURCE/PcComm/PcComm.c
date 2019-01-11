#include  <includes.h>
#include  "SFLib.h"
#include  "halCPU.h"
#include  "halUart.h"
#include  "PcComm.h"
#include  "ByteStream.h"
#include "Led.h"
#include "CommDriver.h"
#include "MainFunc.h"
#include "Func_Common.h"
#include "RealTimer.h"


#define PC_USART USART1
#define PC_USART_IRQHandler USART1_IRQHandler

UartPacketHandler _pcCommUart;

void hal_PcComm_config(void){
    DCB lfDCB;
    _pcCommUart.pUart=PC_USART;
    hal_UART_config(_pcCommUart.pUart);
    
    //��������
	lfDCB.initTypeDef.USART_BaudRate =115200;
	lfDCB.initTypeDef.USART_WordLength = USART_WordLength_8b;
	lfDCB.initTypeDef.USART_StopBits = USART_StopBits_1;
	lfDCB.initTypeDef.USART_Parity = USART_Parity_No;
	lfDCB.initTypeDef.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//USART_HardwareFlowControl_None;//
	lfDCB.initTypeDef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    hal_UART_open(_pcCommUart.pUart,&lfDCB);
}
void drv_PcComm_ResetState(void){
    _pcCommUart.pCRecvTimeout=0;
    fns_SetPcComm_State(PCCOMM_STATE_NOACK);
}

void drv_PcComm_CheckState(UInt32 nowTick){
    if(fns_GetPcComm_State()==PCCOMM_STATE_ACK){
        if(_pcCommUart.rspPacket.CLA!=0){
            UInt8 protocol=0x80; //�޵�ַģʽ
            drv_Comm_sendPacket(_pcCommUart.pUart,&protocol,&_pcCommUart.rspPacket);
        }
        drv_PcComm_ResetState();
        drv_Comm_startRx(&_pcCommUart);
    }else if((_pcCommUart.pCRecvTimeout>0)&&(nowTick>_pcCommUart.pCRecvTimeout)){
        // drv_Comm_sendPacket(_pcCommUart.pUart,&protocol,&_pcCommUart.rspPacket);
        drv_PcComm_ResetState();
        drv_Comm_startRx(&_pcCommUart);
    }
}

void drv_PcComm_onEvent(void){//���ݽ���
  
    UART_PACKET_PARSER parser;

    memset(&_pcCommUart.cmdPacket,0,sizeof(FV_PACKET));
    parser.state=UART_PACKETSTATE_IDLE;    
    parser.stream=(UInt8*)&_pcCommUart.cmdPacket;
    parser.eof=0;
    
    drv_Comm_parseSTBPacket(&parser,_pcCommUart.stream+1,_pcCommUart.eof-1);

    if(parser.state==UART_PACKETSTATE_OK){//��������Packet,���¶Ըð�������
        if( FVCLA_GETTYPE(_pcCommUart.cmdPacket.CLA)==FVTYPE_COMMAND ){
            drv_Comm_onCommand(&_pcCommUart.cmdPacket,&_pcCommUart.rspPacket);
        }
    }else{
        //����ָ��,����    
    }
}

void drv_PcComm_stopRx(){
    _pcCommUart.pCRecvTimeout=drv_Time_getTick()+PCCOM_RECV_TIMOUT;
    hal_UART_stopRx(_pcCommUart.pUart);
}

void isr_PcComm_onRx(UInt8 dr){
     static UInt32 timeout=0;
    UInt32 now=isr_Time_getTick();
    //�����ʱ,��һ���ַ�������CHAR_STB
    if( now > timeout){
        if(dr==CHAR_STB){
            _pcCommUart.stream[0]=dr;
            _pcCommUart.eof=1;
            timeout=now+UART_CHAR_INTERVAL;
            return;
        }
        if(dr!=CHAR_STB) return;
        _pcCommUart.eof=0;
    }
    timeout=now+UART_CHAR_INTERVAL;
    _pcCommUart.stream[_pcCommUart.eof++]=dr;

    if(dr==CHAR_ETB){
        if(_pcCommUart.stream[0]==CHAR_STB){
            drv_PcComm_stopRx();
            isr_Event_raise(EVENT_RX_PCCOMM);
        }else{
            _pcCommUart.eof=0;
        }        
    }
}

void PC_USART_IRQHandler(void){
    if( USART1->SR & USART_FLAG_ORE ){
        //���,�������
        USART1->DR;
    }else if( USART1->SR & USART_FLAG_RXNE ){
        isr_PcComm_onRx(USART1->DR);
    }else{
        USART1->SR &= ~0x0F;
    }
}