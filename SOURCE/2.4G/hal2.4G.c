#include  <includes.h>
#include  "SFLib.h"
#include  "halCPU.h"
#include "halUart.h"
#include  "hal2.4G.h"
#include  "ByteStream.h"
#include "Led.h"
//#include  "tagReceiver.h"

#define LF_USART USART3
#define LF_USART_IRQHandler USART3_IRQHandler

#define NRF_Power_On()    GPIOB_ODR_BIT(12)=1;  //ONBT-PB12
#define NRF_Power_Off()   GPIOB_ODR_BIT(12)=0;  

void hal_LfUart_openPort(void){
    DCB lfDCB;
 
    //串口设置
	lfDCB.initTypeDef.USART_BaudRate =115200;
	lfDCB.initTypeDef.USART_WordLength = USART_WordLength_8b;
	lfDCB.initTypeDef.USART_StopBits = USART_StopBits_1;
	lfDCB.initTypeDef.USART_Parity = USART_Parity_No;
	lfDCB.initTypeDef.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//USART_HardwareFlowControl_None;//
	lfDCB.initTypeDef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    hal_UART_open(LF_USART,&lfDCB);
    hal_UART_startRx(LF_USART);
    
}

void hal_24G_config(void){
	GPIO_InitTypeDef  gpio_init;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	gpio_init.GPIO_Pin  = GPIO_Pin_12;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio_init);

    hal_UART_config(LF_USART);
    hal_LfUart_openPort();
    NRF_Power_On();
}

void hal_24G_powerOff(void){
    //断电
    NRF_Power_Off(); //注意高低电平上电
    //同时让串口所有管脚全部拉低
    OS_LOCK();
    hal_UART_unconfig(LF_USART); //和2.4G通信串口 
    OS_UNLOCK();
    
    delay_ms(2000);

}

void hal_24G_reset(void){
    hal_24G_powerOff();
    //上电
    hal_UART_config(LF_USART);
    NRF_Power_On();    

    delay_ms(300);
}

void USART3_IRQHandler(void){
    if( LF_USART->SR & USART_FLAG_ORE ){
        //溢出,清除数据
        LF_USART->DR;
    }else if( LF_USART->SR & USART_FLAG_RXNE ){
        drv_Led_start(LED_2,1,500,0);
        // m_TagUart_buffer[m_TagUart_eof%LF_USART_BUFFER_CAPACITY]=LF_USART->DR;
        // m_TagUart_eof;
    }else{
        LF_USART->SR &= ~0x0F;
    }
}
