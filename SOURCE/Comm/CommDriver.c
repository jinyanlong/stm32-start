#include "includes.h"
#include "CommDriver.h"
#include "Func_Common.h"
#include "RealTimer.h"
#include "FileFunc.h"
#include "halCPU.h"
#include "RaceTask.h"
#include "SFile.h"
#include "halUart.h"

extern NrfUartPacketHandler _nrfCommUart;

void drv_Comm_sendByte(USART_TypeDef * pUart,UInt8 ch){
    if( (ch==CHAR_STB) || (ch==CHAR_ETB) || (ch==CHAR_ESC) ){
        hal_UART_sendByte(pUart,CHAR_ESC);
        hal_UART_sendByte(pUart,ch | 0x80);
    }else{
        hal_UART_sendByte(pUart,ch);
    }
}

void drv_Comm_sendBytes(USART_TypeDef * pUart,void* pDataBuff ,int nCount){
    int i;
    UInt8* pByteBuff=(UInt8*)pDataBuff;
    for(i=0;i<nCount;i++){
        drv_Comm_sendByte(pUart,pByteBuff[i]);
    }
}
void drv_Comm_sendEmptyPacket(USART_TypeDef * pUart,UInt8* head){
    //�޵�ַ
    UInt8 headSize,crc8;
    
    headSize=1;
    if(head[0]& 0x40){
        headSize+=4;
    }
    if(head[0]& 0x20){
        headSize+=4;
    }
    crc8= crc8_byte_calc(CRC8_PRESET,head,headSize);
    
    hal_UART_sendByte(pUart,CHAR_STB);   
    hal_UART_sendByte(pUart,head[0]);
    if(head[0]& 0x40){
        drv_Comm_sendBytes(pUart,head+1,4);     //������ַ
    }
    if(head[0]& 0x20){
        drv_Comm_sendBytes(pUart,head+4,4);     //�ӻ���ַ
    }
    drv_Comm_sendByte(pUart,crc8);
    hal_UART_sendByte(pUart,CHAR_ETB);
}

void drv_Comm_sendPacket(USART_TypeDef * pUart,UInt8* head,FV_PACKET* pFVPacket){
    //�޵�ַ
    UInt8 headSize,crc8;
    
    headSize=1;
    if(head[0]& 0x40){
        headSize+=4;
    }
    if(head[0]& 0x20){
        headSize+=4;
    }
    crc8= crc8_byte_calc(CRC8_PRESET,head,headSize);
    crc8=crc8_byte_calc(crc8,((UInt8*)pFVPacket),pFVPacket->LEN+4);
    
    hal_UART_sendByte(pUart,CHAR_STB);   
    hal_UART_sendByte(pUart,head[0]);
    if(head[0]& 0x40){
        drv_Comm_sendBytes(pUart,head+1,4);     //������ַ
    }
    if(head[0]& 0x20){
        drv_Comm_sendBytes(pUart,head+4,4);     //�ӻ���ַ
    }
    drv_Comm_sendBytes(pUart,pFVPacket,pFVPacket->LEN+4);
    drv_Comm_sendByte(pUart,crc8);
    hal_UART_sendByte(pUart,CHAR_ETB);
}

HRESULT drv_Comm_checkPacket(FV_PACKET* pFVPacket,UInt32 nLength){    
    //���ݳ��ȼ���
    if( nLength<(pFVPacket->LEN+4)  ){
        return UART_PACKETSTATE_ERR_LEN;
    }

    return UART_PACKETSTATE_OK;
}

//������STB,ETB
void drv_Comm_parseSTBPacket(UART_PACKET_PARSER* parser,UInt8* buffer,UInt16 len){
    UInt16 i;
    UInt8 crc8;
    parser->state=UART_PACKETSTATE_START;
    parser->eof=0;
    parser->protocol=buffer[0];
    if(parser->protocol!=0x80){
        parser->state=UART_PACKETSTATE_ERROR;
        return;
    }
    parser->state=UART_PACKETSTATE_DATA;
    for(i=1;i<len-1;i++){
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
 
    if( (parser->state!=UART_PACKETSTATE_DATA) || (parser->eof<1) ){
        parser->state=UART_PACKETSTATE_ERROR;
        return;
    }
        
    crc8=crc8_byte_calc(CRC8_PRESET,&parser->protocol,1);
    if(parser->eof>1){
        crc8=crc8_byte_calc(crc8,parser->stream,parser->eof-1);
    }
    if(crc8!=parser->stream[parser->eof-1]){
            parser->state=UART_PACKETSTATE_ERR_CRC;
            return;
    }
    if(parser->eof==1){ //�հ�
        parser->state=UART_PACKETSTATE_MASTER;
    }else{
        parser->state=UART_PACKETSTATE_OK;
    }
}


void drv_Comm_parseETBPacket(UART_PACKET_PARSER* parser,UInt8* buffer,UInt16 len){
    if( buffer[0]==CHAR_ETB){
        parser->state=UART_PACKETSTATE_SLAVER;
    }else{
        parser->state=UART_PACKETSTATE_ERROR;
    }
}


void drv_Comm_startRx(UartPacketHandler* pUartHandler){
    //�ͷŴ���
    pUartHandler->pCRecvTimeout=0;
    pUartHandler->eof=0;
    hal_UART_startRx(pUartHandler->pUart);
}


extern volatile MONITOR s_monitor;
extern UInt32 s_deviceID;
extern const SFILE s_dataFile;


// //�жϴ���Ҫ�����ܼ��,�����������æ,���ҷ���CHAR_ETB,���̷�����Ϣ�������߳�
// void isr_Comm_onRx(UartPacketHandler* pUartHandler,UInt8 dr){
//     static UInt32 timeout=0;
//     UInt32 now=isr_Time_getTick();
//     //�����ʱ,��һ���ַ�������CHAR_STB
//     if( now > timeout){
//         if(dr==CHAR_ETB){
//             pUartHandler->stream[0]=dr;
//             pUartHandler->eof=1;
//             timeout=now+UART_CHAR_INTERVAL;
//             return;
//         }
//         if(dr!=CHAR_STB) return;
//         pUartHandler->eof=0;

//     }
//     timeout=now+UART_CHAR_INTERVAL;
//     if(pUartHandler->eof < UART_BUFFER_MAXSIZE){
//         if( (pUartHandler->eof>0) && (pUartHandler->stream[0]==CHAR_ETB) && (CHAR_ETB!=dr) ){
//             pUartHandler->eof=0;
//             timeout=0;
//             return;
//         }else{
//             pUartHandler->stream[pUartHandler->eof++]=dr;
//         }
        
//     }

//     if( (dr==CHAR_ETB) && ((pUartHandler->stream[0]==CHAR_STB) 
//         || ( (pUartHandler->stream[0]==CHAR_ETB) && (pUartHandler->eof>=3) ) ) ){
//         //�ر��ն�,ֱ������ָ��ִ�����
        
//         hal_UART_stopRx(pUartHandler->pUart);
// #ifdef __RS485
//         hal_UART_stopRx(pUartHandler->pUartRS485);
// #endif
//         //�����жϸ���OS�ж�, ������󴥷�һ�����ж�,�ص�OS
//         isr_Event_raise(EVENT_ISR_RX_PCCOMM);
//     }

// }



// //ͨѶ��
// void USART1_IRQHandler(void) {
//     if( USART1->SR & USART_FLAG_ORE ){
//         //���,�������
//         USART1->DR;
//     }else if( USART1->SR & USART_FLAG_RXNE ){
//         isr_Comm_onRx(&_nrfCommUart,USART1->DR);
//     }else{
//         USART1->SR &= ~0x0F;
//     }
// }

// #ifdef __BOARD_181213
// void USART2_IRQHandler(void){
//     if( USART2->SR & USART_FLAG_ORE ){
//         //���,�������
//         USART2->DR;
//     }else if( USART2->SR & USART_FLAG_RXNE ){
//         isr_Comm_onRx(&_nrfCommUart,USART2->DR);
//     }else if( USART2->SR & USART_FLAG_TC ){    //USART_FLAG_TXE/USART_FLAG_TC//�������
//         USART2->SR &= ~USART_FLAG_TC;  //ֻ��������ֱ��д0���,������ɿ���
//         if(USART2->SR & USART_FLAG_TXE){
//         }
//     }else{
//         USART2->SR = 0;
//     }
// }
// #endif
