#include <includes.h>
#include "halCPU.h"
#include "halUART.h"


//功能：串口pin脚初始化
//入参：串口编号
//返回：无
void hal_UART_config(USART_TypeDef * pUart){
 	GPIO_InitTypeDef  gpio_init;
	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    
	if(pUart==USART1){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);
        
		//UART1:RXD
		gpio_init.GPIO_Pin  = GPIO_Pin_10;
	    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	    GPIO_Init(GPIOA, &gpio_init);
	
		//UART1:TXD
		gpio_init.GPIO_Pin  = GPIO_Pin_9;
	    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
	    GPIO_Init(GPIOA, &gpio_init);
        
        NVIC_SetPriority(USART1_IRQn,ISR_PRIORITY_UART);
        NVIC_EnableIRQ(USART1_IRQn);
        
    }else if(pUart==USART2){
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        
		//UART2:CTS,RXD
		gpio_init.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_3;
	    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	    GPIO_Init(GPIOA, &gpio_init);

		//UART2:RTS
        //GPIOA_ODR_BIT(1)=0;
		gpio_init.GPIO_Pin  = GPIO_Pin_1;
	    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
	    GPIO_Init(GPIOA, &gpio_init);
        
        
		//UART2:TXD
		gpio_init.GPIO_Pin  = GPIO_Pin_2;
	    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
	    GPIO_Init(GPIOA, &gpio_init);
	
        NVIC_SetPriority(USART2_IRQn,ISR_PRIORITY_UART);
        NVIC_EnableIRQ(USART2_IRQn);
    }else if(pUart==USART3){
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
		//UART3:RXD	 GPS-RXD   PB11
		gpio_init.GPIO_Pin  = GPIO_Pin_11;
	    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	    GPIO_Init(GPIOB, &gpio_init);
	
		//UART3:TXD	 GPS-TXD   PB10
		gpio_init.GPIO_Pin  = GPIO_Pin_10;
	    gpio_init.GPIO_Mode = GPIO_Mode_AF_PP;
	    GPIO_Init(GPIOB, &gpio_init);
        
        NVIC_SetPriority(USART3_IRQn,ISR_PRIORITY_UART);
        NVIC_EnableIRQ(USART3_IRQn);
	}
	  
}

void hal_UART_unconfig(USART_TypeDef * pUart){
	GPIO_InitTypeDef  gpio_init;
	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;

	if(pUart==USART1){
        GPIOA_ODR_BIT(9)=GPIOA_ODR_BIT(10)=0;
	    gpio_init.GPIO_Pin  = GPIO_Pin_9|GPIO_Pin_10;
	    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
	    GPIO_Init(GPIOA, &gpio_init);
    }else if(pUart==USART2){
        GPIOA_ODR_BIT(0)=GPIOA_ODR_BIT(1)=GPIOA_ODR_BIT(2)=GPIOA_ODR_BIT(3)=0;
		gpio_init.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
	    GPIO_Init(GPIOA, &gpio_init);
    }else if(pUart==USART3){
        GPIOC_ODR_BIT(10)=GPIOA_ODR_BIT(11)=0;
		gpio_init.GPIO_Pin  = GPIO_Pin_10|  GPIO_Pin_11;
	    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
	    GPIO_Init(GPIOB, &gpio_init);
	}
}
//功能：初始化串口
//入参：串口编号，串口的参数设置
//返回：无
void hal_UART_open(USART_TypeDef * pUart,DCB* dcb){ 
    USART_Cmd(pUart, DISABLE);
    USART_Init(pUart, &dcb->initTypeDef);
    USART_Cmd(pUart, ENABLE);
}
void hal_UART_close(USART_TypeDef * pUart){
    USART_Cmd(pUart, DISABLE);
}    

//同步发送,直到完成 
void hal_UART_sendByte(USART_TypeDef * pUart,int bytVal){
    pUart->DR=bytVal;
	while(( pUart->SR & USART_FLAG_TC) == 0);  //CTS/RTS模式下这样的代码可能会导致死机或长时间等待,所以任务优先级一定要低
}

void hal_UART_sendBytes(USART_TypeDef * pUart,const void* pBuff,UInt32 nCount){
	UInt32 i;
	for(i=0;i<nCount;i++){
        hal_UART_sendByte(pUart,((UInt8*)pBuff)[i]);
	}
}

void hal_UART_sendString(USART_TypeDef * pUart,String pBuff){
	UInt32 i;
	for(i=0;((UInt8*)pBuff)[i]!='\0';i++){
        hal_UART_sendByte(pUart,((UInt8*)pBuff)[i]);
	}
}


void hal_UART_startRx(USART_TypeDef * pUart){   
    //清空缓冲区,开始接收
    USART_ITConfig(pUart,USART_IT_RXNE,DISABLE);
	if(pUart->SR & USART_FLAG_RXNE){
    	pUart->DR;	//清除接收缓冲区
	}
    USART_ITConfig(pUart,USART_IT_RXNE,ENABLE);
}
void hal_UART_stopRx(USART_TypeDef * pUart){
	USART_ITConfig(pUart,USART_IT_RXNE,DISABLE);
	if(pUart->SR & USART_FLAG_RXNE){
    	pUart->DR;	//清除接收缓冲区
	}
}





