#include  <includes.h>
#include  "SFLib.h"
#include  "halCPU.h"
#include  "halUart.h"
#include  "halNrf.h"
#include  "ByteStream.h"
#include "Led.h"
#include "CommDriver.h"
#include "Func_Common.h"
#include "RealTimer.h"

#define LF_USART USART3
#define LF_USART_IRQHandler USART3_IRQHandler

#define NRF_Power_On()    GPIOB_ODR_BIT(12)=1;  //ONBT-PB12
#define NRF_Power_Off()   GPIOB_ODR_BIT(12)=0;  

NrfUartPacketHandler _nrfCommUart;
extern UartPacketHandler _pcCommUart;

void hal_NrfPin_config(void){
    GPIO_InitTypeDef  gpio_init;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin  = GPIO_Pin_12;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio_init);
}

void hal_NrfUart_config(void){
    DCB lfDCB;
    _nrfCommUart.pUart=LF_USART;
    _nrfCommUart.eof=0;
    hal_UART_config(_nrfCommUart.pUart);
    

    //��������
	lfDCB.initTypeDef.USART_BaudRate =115200;
	lfDCB.initTypeDef.USART_WordLength = USART_WordLength_8b;
	lfDCB.initTypeDef.USART_StopBits = USART_StopBits_1;
	lfDCB.initTypeDef.USART_Parity = USART_Parity_No;
	lfDCB.initTypeDef.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//USART_HardwareFlowControl_None;//
	lfDCB.initTypeDef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    hal_UART_open(_nrfCommUart.pUart,&lfDCB);
}

void hal_Nrf_powerOff(void){
    //�ϵ�
    NRF_Power_Off(); //ע��ߵ͵�ƽ�ϵ�
    //ͬʱ�ô������йܽ�ȫ������
    OS_LOCK();
    hal_UART_unconfig(LF_USART); //��2.4Gͨ�Ŵ��� 
    OS_UNLOCK();
}


void hal_Nrf_config(void){
    hal_NrfPin_config();
    hal_NrfUart_config();
    hal_Nrf_powerOff();
}


void drv_NRF_init(void){
    hal_Nrf_powerOff();
    delay_us(30000);
    //�ϵ�
    hal_UART_config(LF_USART);
    NRF_Power_On();

    delay_us(30000);
}



void drv_NrfComm_parsePacket(UART_PACKET_PARSER* parser,UInt8* buffer,UInt16 len){
    UInt16 i;
    UInt8 crc8;
    parser->state=UART_PACKETSTATE_START;
    parser->eof=0;

    parser->state=UART_PACKETSTATE_DATA;
    for(i=0;i<len;i++){
        UInt8 val=buffer[i];
        
        if(UART_PACKETSTATE_ESC==parser->state){
            //Ԥ����
            val=val&0x7F;
            parser->state=UART_PACKETSTATE_DATA;
        }else{ //UART_PACKETSTATE_DATA
            if(val==CHAR_ESC){
                parser->state=UART_PACKETSTATE_ESC;
                continue;
            }
        }
        
        parser->stream[parser->eof]=val;
        parser->eof++;
    }
 
    if( (parser->state!=UART_PACKETSTATE_DATA) || (parser->eof<2) ){
        parser->state=UART_PACKETSTATE_ERROR;
        return;
    }
   
    crc8=crc8_byte_calc(CRC8_PRESET,parser->stream,parser->eof-1);
    
    if(crc8!=parser->stream[parser->eof-1]){
        parser->state=UART_PACKETSTATE_ERR_CRC;
        return;
    }

    parser->state=UART_PACKETSTATE_OK;
}

void drv_nrfComm_startRx(NrfUartPacketHandler* pUartHandler){
    pUartHandler->eof=0;
    hal_UART_startRx(pUartHandler->pUart);
}

#define NRFEVENT_RESULT_OK      0
#define NRFEVENT_RESULT_IGNORE  1    
#define NRFEVENT_RESULT_FAILED  2    

UInt8 drv_NrfComm_onEvent_(void){ //��������
    UART_PACKET_PARSER parser;

    //����֡����
    memset(&_nrfCommUart.cmdPacket,0,sizeof(NRFFV_PACKET));
    parser.state=UART_PACKETSTATE_IDLE;    
    parser.stream=(UInt8*)&_nrfCommUart.cmdPacket;
    parser.eof=0;
    drv_NrfComm_parsePacket(&parser,_nrfCommUart.stream+1,_nrfCommUart.eof-2);
    if(parser.state!=UART_PACKETSTATE_OK) return NRFEVENT_RESULT_FAILED;

    //�������ݼ��
    if(_pcCommUart.cmdPacket.INS==0x07){ //NRF ����ָ��
        if(_nrfCommUart.cmdPacket.PROT!=0x80) return NRFEVENT_RESULT_IGNORE;  //Э�鲻ƥ��,����
    }else if(_pcCommUart.cmdPacket.INS==0x05){  //��ƵӦ��
        if( (_nrfCommUart.cmdPacket.PROT!=0x81)
            || (_nrfCommUart.cmdPacket.CLA!=LF_NRFACK) 
            || (_nrfCommUart.cmdPacket.INS!=_pcCommUart.cmdPacket.PARAMS[1]) ) {
            return NRFEVENT_RESULT_IGNORE;
        }
    }else if(_pcCommUart.cmdPacket.INS==0x06){       //��ƵӦ�� 
        if( (_nrfCommUart.cmdPacket.PROT!=0x81)
            || (_nrfCommUart.cmdPacket.CLA!=NRF_TAG2MCU) 
            || (_nrfCommUart.cmdPacket.INS!=_pcCommUart.cmdPacket.PARAMS[3]) ) {
            return NRFEVENT_RESULT_IGNORE;
        }
    }else{
        return NRFEVENT_RESULT_IGNORE;
    }
    
    //ִ��һЩ���ӵĴ�����
    if( (4+2+_nrfCommUart.cmdPacket.LEN+1)!=parser.eof ) {
        return NRFEVENT_RESULT_FAILED;
    }
    _pcCommUart.rspPacket.LEN=_nrfCommUart.cmdPacket.LEN+2;
    memcpy(_pcCommUart.rspPacket.PARAMS,parser.stream+4,_pcCommUart.rspPacket.LEN);

    return NRFEVENT_RESULT_OK;
}
void drv_NrfComm_onEvent(void){ //��������
    UInt8 result;
    if(_pcCommUart.lazyMode){
        result=drv_NrfComm_onEvent_();
        if(result==NRFEVENT_RESULT_IGNORE){
            return; //�����ȴ�
        }else if(result==NRFEVENT_RESULT_FAILED){
            drv_CommFunc_setError(&_pcCommUart.rspPacket,ERRCODE_LF_BUSY);
        }//else OK
        
        if(_pcCommUart.rspPacket.CLA!=0){
            UInt8 protocol=0x80; //�޵�ַģʽ
            drv_Comm_sendPacket(_pcCommUart.pUart,&protocol,&_pcCommUart.rspPacket);
        }
        drv_Comm_startRx(&_pcCommUart); //���¿���PC-UART����
    }
    

}
    

void isr_NrfComm_onRx(UInt8 dr){
     static UInt32 timeout=0;
    UInt32 now=isr_Time_getTick();
    //�����ʱ,��һ���ַ�������CHAR_STB
    if( now > timeout){
        if(dr==CHAR_STB){
            _nrfCommUart.stream[0]=dr;
            _nrfCommUart.eof=1;
            timeout=now+UART_CHAR_INTERVAL;
            return;
        }
        if(dr!=CHAR_STB) return;
        _nrfCommUart.eof=0;

    }
    timeout=now+UART_CHAR_INTERVAL;
    if(_nrfCommUart.eof < UART_BUFFER_MAXSIZE){
        _nrfCommUart.stream[_nrfCommUart.eof++]=dr; 
    }

    if(dr==CHAR_ETB){
        if((_nrfCommUart.eof>1)&&(_nrfCommUart.stream[0]==CHAR_STB)){
            //�ر��ն�,ֱ������ָ��ִ�����
            hal_UART_stopRx(_nrfCommUart.pUart);
            if(_pcCommUart.lazyMode){
                isr_Event_raise(EVENT_RX_NRFCOMM);
            }else{
                drv_nrfComm_startRx(&_nrfCommUart);
            }
        }else{
            _nrfCommUart.eof=0;
        }
    }
}

void LF_USART_IRQHandler(void){
    if( LF_USART->SR & USART_FLAG_ORE ){
        //���,�������
        LF_USART->DR;
    }else if( LF_USART->SR & USART_FLAG_RXNE ){
        isr_NrfComm_onRx(LF_USART->DR);
    }else{
        LF_USART->SR &= ~0x0F;
    }
}
