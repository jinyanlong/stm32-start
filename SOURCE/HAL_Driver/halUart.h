#ifndef __HALUART_001
#define __HALUART_001


#define COMMPORT_COUNT 3
//#define COM1 0
//#define COM2 1
//#define COM3 2
typedef struct _DCB{
    USART_InitTypeDef initTypeDef;
}DCB;


void hal_UART_config(USART_TypeDef * pUart);
void hal_UART_unconfig(USART_TypeDef * pUart);
void hal_UART_open(USART_TypeDef * pUart,DCB* dcb);
void hal_UART_close(USART_TypeDef * pUart);

//同步发送,直到完成 
void hal_UART_sendByte(USART_TypeDef * pUart,int bytVal);
void hal_UART_sendBytes(USART_TypeDef * pUart,const void* pBuff,UInt32 nCount);
void hal_UART_sendString(USART_TypeDef * pUart,String pBuff);
void hal_UART_startRx(USART_TypeDef * pUart);
void hal_UART_stopRx(USART_TypeDef * pUart);

void USART1_IRQHandler(void);
//void USART2_IRQHandler(void);
void USART3_IRQHandler(void);


#endif

