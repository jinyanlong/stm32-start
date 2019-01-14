#include <includes.h>
#include "halLF.h"

#define SEND_HZ  260  //50ms 发送一次数据

#define LF_READY_SEND   0
#define LF_SEND_START   1
#define LF_SEND_END     2

#define LF_DOUT_PINNO           7
#define LF_DIN_PINNO            6
#define LFTIM3_IRQHandler  TIM3_IRQHandler
//DIN反向
#define LF_DOUT_IS_HIGH()    GPIOB_IDR_BIT(LF_DOUT_PINNO)==1
#define LF_DOUT_SET_LOW()    GPIOB_ODR_BIT(LF_DOUT_PINNO)=0
#define LF_DOUT_SET_HIGH()   GPIOB_ODR_BIT(LF_DOUT_PINNO)=1

void hal_LF_config(void){//din-pb6|dout-pb7|sck-pb8
	GPIO_InitTypeDef  gpio_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = (1<<LF_DOUT_PINNO)| (1<<LF_DIN_PINNO);
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio_init);
	      
}


void drv_LFtim3_init(void){
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
    
    //定时器3设置 ，定时192*72/72=192us,是数据周期384us的一半|数据频率2.6kHZ   
	TIM_TimeBaseStructure.TIM_Period = 191;		      
	TIM_TimeBaseStructure.TIM_Prescaler = 71;    	    
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	//向上计数        
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
        	
	TIM_ARRPreloadConfig(TIM3, ENABLE);          /*使能预装载*/
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	 /*预先清除所有中断位*/
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);   /* 4个通道和溢出都配置中断*/
	TIM_Cmd(TIM3, ENABLE);  	                /* 允许TIM3开始计数 */
}


unsigned char m_MC_buf[90]=  //通道一//@@1
{
0,
1,0,1,0,//（*）原配置为0,1,0 这里可以做调整 后觉得该为最少六位premble,改变preamble的长度可以影响到标签复位时钟的个数，减少preamble收到复位时钟也减少，反之则反
0,0,
0,1,1,0,1,0,1,0,// 6A //7
1,0,0,1,0,1,0,1,// 95  //8
0,0,
};

void IdToBuff(u16 id, unsigned char* buffer){
#ifdef  __LF_CRC16
    u16 crc16;
    memcpy(buffer,&id,2);
    crc16=crc16_ccitt_l(0xF5A3,buffer,2);
    memcpy(buffer+2,&crc16,2);
#else
    u8 crc8;
    buffer[0]=(u8)(id>>8);
    buffer[1]=(u8)(id);
    crc8=crc8_byte_calc(0x00,buffer,2)&0x0F;
    id|=(u16)crc8<<12;
    buffer[0]=buffer[2]=(u8)(id>>8);
    buffer[1]=buffer[3]=(u8)(id);
#endif
}
void BytesToMC(unsigned char* bytes,unsigned char* mcBuffer){
    int i,j;  
    for(i=0;i<4;i++){
        for(j=0;j<8;j++){
            if(bytes[i]& (0x80>>j) ){ // 高位在前，低位在后
                mcBuffer[(i*8+j)*2]=1;
                mcBuffer[(i*8+j)*2+1]=0;
            }else{
                mcBuffer[(i*8+j)*2]=0;
                mcBuffer[(i*8+j)*2+1]=1;
            }
        }
    }
}

void readyLfSendData(u16 data){
    /*125K激活码为4个字节、前两个字节为pattern,用作AS3933自带的过滤模式，pattern取值并不是随意的，16bit模式下只能选取0(0、1),1(1、0)组合，即6\A\9\5
    32bit模式下取值随意。
    后两字节为我们所传递的信息，后两字节的高四位用作CRC校验，取CRC校验值的低四位。同时为了加强校验，我们选择将传递的信息一次发送两遍，接收端对比收到
    数据前后是否一致来加强校验。
    实际传递有效位为11位，4位用作CRC、1位用于第7、8位保持一致
    */   

    u8 buffer[4];
    IdToBuff(data,buffer);
    BytesToMC(buffer,&m_MC_buf[25]);    
    m_MC_buf[89]=0;
}

void drv_LF_init(void){
    readyLfSendData(0x8011);
    drv_LFtim3_init();
}

void lfSendBit(uint8_t bit){
        if(bit == 1 ){
             LF_DOUT_SET_HIGH(); 
           }       
        else if(bit == 0){
             LF_DOUT_SET_LOW(); 
        }        
}

volatile uint16_t _lfSendCount=0;
volatile uint32_t _lfSendTick=0;
uint8_t _lfSendState=LF_READY_SEND;

bool lfSendStart(uint16_t count){
    if(_lfSendCount==0){
        _lfSendState=LF_READY_SEND;
        _lfSendCount=count;
        return true;
    }else{
        return false;
    }
}

void LFTIM3_IRQHandler(void) //4us timetick  //@@1 192us
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){       
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);             
        
        if(_lfSendCount==0){
            return;
        }
        if(_lfSendState==LF_READY_SEND){
            LF_DOUT_SET_HIGH();//拉高准备发送
            _lfSendTick++;
            if(_lfSendTick==4){
               _lfSendState=LF_SEND_START;
               _lfSendTick=0;
            }
            return;
        }else if(_lfSendState==LF_SEND_START){
            if((_lfSendTick%2)==0){
                lfSendBit(m_MC_buf[_lfSendTick/2]);
            }
            if(_lfSendTick==179){
                _lfSendState=LF_SEND_END;
            }
            _lfSendTick++;
        }else if(_lfSendState==LF_SEND_END){
            LF_DOUT_SET_LOW();
            _lfSendState=LF_READY_SEND;
            _lfSendTick=0;
            _lfSendCount--;
        }
    }
}
