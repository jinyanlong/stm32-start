#include <includes.h>
#include "halLF.h"

#define LF_IRQHandler  TIM3_IRQHandler

#define LF_AC1_PIN 6
#define LF_AC2_PIN 5
#define LF_AC3_PIN 4
#define LF_AC4_PIN 3
#define LF_BC1_PIN 7
#define LF_BC2_PIN 8
#define LF_BC3_PIN 9
#define LF_BC4_PIN 13
#define LF_MC1_PIN 9
#define LF_MC2_PIN 8
#define LF_MC3_PIN 7
#define LF_MC4_PIN 6
#define LF_NC1_PIN 8
#define LF_NC2_PIN 11
#define LF_NC3_PIN 12
#define LF_NC4_PIN 15

__INLINE bool LF_DOUT_IS_HIGH(uint8_t ant){
    bool result=false;
    switch(ant){
        case LF_ANTA:
            result=(GPIOC_IDR_BIT(LF_ANTA_DOUTPIN)==1);
            break;
        case LF_ANTB:
            result=(GPIOC_IDR_BIT(LF_ANTM_DOUTPIB)==1);
            break;
        case LF_ANTM:
            result=(GPIOC_IDR_BIT(LF_ANTM_DOUTPIN)==1);
            break;
        case LF_ANTN:        
            result=(GPIOA_IDR_BIT(LF_ANTN_DOUTPIN)==1);
            break;
        default:
            break;
    }
    return result;
}
__INLINE void LF_DOUT_SET_LOW(uint8_t ant){
    switch(ant){
        case LF_ANTA:
            GPIOC_ODR_BIT(LF_ANTA_DOUTPIN)=0;
            break;
        case LF_ANTB:
            GPIOC_ODR_BIT(LF_ANTM_DOUTPIB)=0;
            break;
        case LF_ANTM:
            GPIOC_ODR_BIT(LF_ANTM_DOUTPIN)=0;
            break;
        case LF_ANTN:        
            GPIOA_ODR_BIT(LF_ANTN_DOUTPIN)=0;
            break;
        default:
            break;
    }
}
__INLINE void LF_DOUT_SET_HIGH(uint8_t ant){
    switch(ant){
        case LF_ANTA:
            GPIOC_ODR_BIT(LF_ANTA_DOUTPIN)=1;
            break;
        case LF_ANTB:
            GPIOC_ODR_BIT(LF_ANTM_DOUTPIB)=1;
            break;
        case LF_ANTM:
            GPIOC_ODR_BIT(LF_ANTM_DOUTPIN)=1;
            break;
        case LF_ANTN:        
            GPIOA_ODR_BIT(LF_ANTN_DOUTPIN)=1;
            break;
        default:
            break;
    }
}


void hal_LF_OutPin_config(void){//pc14-A|pc12-b|pc9-M|pa7-n
	GPIO_InitTypeDef  gpio_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);

    LF_DOUT_SET_LOW(LF_ANTA);
    LF_DOUT_SET_LOW(LF_ANTB);
    LF_DOUT_SET_LOW(LF_ANTM);
    LF_DOUT_SET_LOW(LF_ANTN);

	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = (1<<LF_ANTA_DOUTPIN)| (1<<LF_ANTB_DOUTPIN)| (1<<LF_ANTM_DOUTPIN);
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &gpio_init);

    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = (1<<LF_ANTN_DOUTPIN);
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &gpio_init);      
}

void hal_LF_SetCapPin(uint8_t ant,uint8_t val){
    switch(ant){
        case LF_ANTA:
            GPIOB_ODR_BIT(LF_AC1_PIN)=((val)&(0x01));
            GPIOB_ODR_BIT(LF_AC2_PIN)=((val)&(0x02));
            GPIOB_ODR_BIT(LF_AC3_PIN)=((val)&(0x04));
            GPIOB_ODR_BIT(LF_AC4_PIN)=((val)&(0x08));
            break;
        case LF_ANTB:
            GPIOB_ODR_BIT(LF_BC1_PIN)=((val)&(0x01));
            GPIOB_ODR_BIT(LF_BC2_PIN)=((val)&(0x02));
            GPIOB_ODR_BIT(LF_BC3_PIN)=((val)&(0x04));
            GPIOC_ODR_BIT(LF_BC4_PIN)=((val)&(0x08));
            break;
        case LF_ANTM:
            GPIOC_ODR_BIT(LF_MC1_PIN)=((val)&(0x01));
            GPIOC_ODR_BIT(LF_MC2_PIN)=((val)&(0x02));
            GPIOC_ODR_BIT(LF_MC3_PIN)=((val)&(0x04));
            GPIOC_ODR_BIT(LF_MC4_PIN)=((val)&(0x08));
            break;
        case LF_ANTN:        
            GPIOA_ODR_BIT(LF_NC1_PIN)=((val)&(0x01));
            GPIOA_ODR_BIT(LF_NC2_PIN)=((val)&(0x02));
            GPIOA_ODR_BIT(LF_NC3_PIN)=((val)&(0x04));
            GPIOA_ODR_BIT(LF_NC4_PIN)=((val)&(0x08));
            break;
        default:
            break;
    }
}

//pb6-a1|pb5-a2|pb4-a3|pb3-a4|pb7-b1|pb8-b2|pb9-b3|pc13-b4|
//pc9-m1|pc8-m2|pc7-m3|pc6-m4|pa8-n1|pa11-n2|pa12-n3|pa15-n4|
void hal_LF_CapPin_config(void){
	GPIO_InitTypeDef  gpio_init;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);

    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);//完全禁用SWD及JTAG 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);     //禁用JTAG

    hal_LF_SetCapPin(LF_ANTA,0x00);
    hal_LF_SetCapPin(LF_ANTB,0x00);
    hal_LF_SetCapPin(LF_ANTM,0x00);
    hal_LF_SetCapPin(LF_ANTN,0x00);

	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = (1<<LF_NC1_PIN)|(1<<LF_NC2_PIN)|(1<<LF_NC3_PIN)|(1<<LF_NC4_PIN);
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &gpio_init);

    gpio_init.GPIO_Pin  = (1<<LF_AC1_PIN)|(1<<LF_AC2_PIN)|(1<<LF_AC3_PIN)|(1<<LF_AC4_PIN)| \
                          (1<<LF_BC1_PIN)|(1<<LF_BC1_PIN)|(1<<LF_BC1_PIN);
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &gpio_init);  

    gpio_init.GPIO_Pin  = (1<<LF_BC4_PIN)|(1<<LF_MC1_PIN)|(1<<LF_MC2_PIN)|(1<<LF_MC3_PIN)|(1<<LF_MC4_PIN);
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &gpio_init);      
}

void hal_LF_config(void){
    hal_LF_OutPin_config();//信号输出引脚配置
    hal_LF_CapPin_config();//电容调节引脚配置
}


void hal_LF_initTimer(void){
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
    
    //定时器3设置 ，定时192*72/72=192us,是数据周期384us的一半|数据频率2.6kHZ   
	TIM_TimeBaseStructure.TIM_Period = 188*2; //125K模式下的参数
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
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);   /* 4个通道和溢出都配置中断*/
}

static __INLINE void hal_LF_startTimer(void){
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	 /*预先清除所有中断位*/
    TIM_Cmd(TIM3, ENABLE);  	                /* 允许TIM3开始计数 */
}
static __INLINE void hal_LF_stopTimer(void){
    TIM_Cmd(TIM3, DISABLE);
}

//设置输出管脚电平
static __INLINE void LF_SETBIT(uint8_t ant,uint8_t bit){
    if(bit == 1 ){
         LF_DOUT_SET_HIGH(ant); 
    }else if(bit == 0){
         LF_DOUT_SET_LOW(ant); 
    }        
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static volatile uint8_t _buffer[32];
 uint32_t volatile* m_phase_buffer;
volatile uint32_t m_phase_sof; 
volatile uint32_t m_phase_eof; 

#define PHRASE_ON  1
#define PHRASE_OFF 0
static __INLINE void drv_MCBuff_pushBit(uint8_t bit){
    if(bit){
        m_phase_buffer[m_phase_eof++]=PHRASE_ON;
        m_phase_buffer[m_phase_eof++]=PHRASE_OFF;
    }else{
        m_phase_buffer[m_phase_eof++]=PHRASE_OFF;
        m_phase_buffer[m_phase_eof++]=PHRASE_ON;
    }
}

//推送位相(半个位)
static __INLINE void drv_MCBuff_pushPhrase(bool phase,int count){
    int i;
    for(i=0;i<count;i++){
        if(phase){
            m_phase_buffer[m_phase_eof++]=PHRASE_ON;
        }else{
            m_phase_buffer[m_phase_eof++]=PHRASE_OFF;
        }
    }
}

//为保持兼容,这里用的是高位在前,低位在后
static __INLINE void drv_MCBuff_pushByte(uint8_t bytVal){
    int i;
    for(i=0;i<8;i++){
        drv_MCBuff_pushBit(bytVal & (0x80>>i));
    }
}

static __INLINE void drv_MCBuff_pushUInt16(uint16_t uVal){
    drv_MCBuff_pushByte((uint8_t)uVal);
    drv_MCBuff_pushByte((uint8_t)(uVal>>8));
}

void drv_MCBuff_reset(void){
    m_phase_sof=m_phase_eof=0;
    memset((void*)_buffer,0,sizeof(_buffer));
}

void drv_MCBuff_create(uint16_t coinId){
    drv_MCBuff_reset();
    
    //保持一段时间的射频开启状态
    drv_MCBuff_pushPhrase(PHRASE_ON,5);

    //起始位 (1 pharse)
    drv_MCBuff_pushPhrase(PHRASE_OFF,1);
    
    //premble (6+2 pharse)
    drv_MCBuff_pushBit(1);
    drv_MCBuff_pushBit(1);
    drv_MCBuff_pushBit(1);
    drv_MCBuff_pushPhrase(PHRASE_ON,2);
    
    //patten(1B,=0x78) (16+2 phase)
    drv_MCBuff_pushByte(0x78);
    if(coinId&0x80){
        drv_MCBuff_pushPhrase(PHRASE_OFF,2);
    }else{
        drv_MCBuff_pushPhrase(PHRASE_ON,2);
    }
    
    //内容(4B)  (64+2 phase)
    drv_MCBuff_pushUInt16(coinId);
    drv_MCBuff_pushUInt16(crc16_ccitt_l(0xF5A3,&coinId,2));
    drv_MCBuff_pushPhrase(PHRASE_OFF,1);

}

void drv_LF_init(void){
    m_phase_buffer=PTR_BITADDR(_buffer,0);
    drv_MCBuff_reset();
    hal_LF_initTimer();
}
void drv_LF_wait(void){
    while(m_phase_sof<m_phase_eof){
        __nop();
    }
}
void drv_LF_send(uint16_t coinId){
    drv_MCBuff_create(coinId);
    hal_LF_startTimer();
    drv_LF_wait();
}

void hal_OpenLF(uint8_t ant){
    drv_LF_wait();
    LF_DOUT_SET_HIGH(ant);
}

void hal_CloseLF(uint8_t ant){
    drv_LF_wait();
    LF_DOUT_SET_LOW(ant);
}


void LF_IRQHandler(void){
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){       
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        
        if(m_phase_sof<m_phase_eof){
            LF_SETBIT(LF_ANTA,m_phase_buffer[m_phase_sof++]);
        }
        
        if(m_phase_sof>=m_phase_eof){
            hal_LF_stopTimer();
        }
    }
}



