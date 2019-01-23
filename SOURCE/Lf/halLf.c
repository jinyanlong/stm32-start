#include <includes.h>
#include "halLF.h"

#define LF_DOUTA_PINNO           14
#define LF_DOUTB_PINNO           12
#define LF_DOUTM_PINNO           10
#define LF_DOUTN_PINNO           7
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

__INLINE bool LF_DOUT_IS_HIGH(uint8_t pin){
    bool result=false;
    switch(pin){
        case LF_DOUTA_PINNO:
        case LF_DOUTB_PINNO:
        case LF_DOUTM_PINNO:
            result=(GPIOC_IDR_BIT(pin)==1);
            break;
        case LF_DOUTN_PINNO:        
            result=(GPIOA_IDR_BIT(pin)==1);
            break;
        default:
            break;
    }
    return result;
}
__INLINE void LF_DOUT_SET_LOW(uint8_t pin){
    switch(pin){
        case LF_DOUTA_PINNO:
        case LF_DOUTB_PINNO:
        case LF_DOUTM_PINNO:
            GPIOC_ODR_BIT(pin)=0;
            break;
        case LF_DOUTN_PINNO:        
            GPIOA_ODR_BIT(pin)=0;
            break;
        default:
            break;
    }
}
__INLINE void LF_DOUT_SET_HIGH(uint8_t pin){
    switch(pin){
        case LF_DOUTA_PINNO:
        case LF_DOUTB_PINNO:
        case LF_DOUTM_PINNO:
            GPIOC_ODR_BIT(pin)=1;
            break;
        case LF_DOUTN_PINNO:        
            GPIOA_ODR_BIT(pin)=1;
            break;
        default:
            break;
    }
}


void hal_LF_OutPin_config(void){//pc14-A|pc12-b|pc9-M|pa7-n
	GPIO_InitTypeDef  gpio_init;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC,ENABLE);

    LF_DOUT_SET_LOW(LF_DOUTA_PINNO);
    LF_DOUT_SET_LOW(LF_DOUTB_PINNO);
    LF_DOUT_SET_LOW(LF_DOUTM_PINNO);
    LF_DOUT_SET_LOW(LF_DOUTN_PINNO);

	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = (1<<LF_DOUTA_PINNO)| (1<<LF_DOUTB_PINNO)| (1<<LF_DOUTM_PINNO);
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &gpio_init);

    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = (1<<LF_DOUTN_PINNO);
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &gpio_init);      
}

//pb6-a1|pb5-a2|pb4-a3|pb3-a4|pb7-b1|pb8-b2|pb9-b3|pc13-b4|
//pc9-m1|pc8-m2|pc7-m3|pc6-m4|pa8-n1|pa11-n2|pa12-n3|pa15-n4|
void hal_LF_CapPin_config(void){
	GPIO_InitTypeDef  gpio_init;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO,ENABLE);

    // GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);//��ȫ����SWD��JTAG 
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);     //����JTAG

    
    GPIOA_ODR_BIT(LF_NC1_PIN)=0;
    GPIOA_ODR_BIT(LF_NC2_PIN)=0;
    GPIOA_ODR_BIT(LF_NC3_PIN)=0;
    GPIOA_ODR_BIT(LF_NC4_PIN)=0;

    GPIOB_ODR_BIT(LF_AC1_PIN)=0;
    GPIOB_ODR_BIT(LF_AC2_PIN)=0;
    GPIOB_ODR_BIT(LF_AC3_PIN)=0;
    GPIOB_ODR_BIT(LF_AC4_PIN)=0;

    GPIOB_ODR_BIT(LF_BC1_PIN)=0;
    GPIOB_ODR_BIT(LF_BC2_PIN)=0;
    GPIOB_ODR_BIT(LF_BC3_PIN)=0;
    GPIOC_ODR_BIT(LF_BC4_PIN)=0;

    GPIOC_ODR_BIT(LF_MC1_PIN)=0;
    GPIOC_ODR_BIT(LF_MC2_PIN)=0;
    GPIOC_ODR_BIT(LF_MC3_PIN)=0;
    GPIOC_ODR_BIT(LF_MC4_PIN)=0;

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
    hal_LF_OutPin_config();//�ź������������
    hal_LF_CapPin_config();//���ݵ�����������
}


void hal_LF_initTimer(void){
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
    
    //��ʱ��3���� ����ʱ192*72/72=192us,����������384us��һ��|����Ƶ��2.6kHZ   
	TIM_TimeBaseStructure.TIM_Period = 188*2; //125Kģʽ�µĲ���
	TIM_TimeBaseStructure.TIM_Prescaler = 71;    	    
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 	//���ϼ���        
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
        	
	TIM_ARRPreloadConfig(TIM3, ENABLE);          /*ʹ��Ԥװ��*/
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);   /* 4��ͨ��������������ж�*/
}

static __INLINE void hal_LF_startTimer(void){
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);	 /*Ԥ����������ж�λ*/
    TIM_Cmd(TIM3, ENABLE);  	                /* ����TIM3��ʼ���� */
}
static __INLINE void hal_LF_stopTimer(void){
    TIM_Cmd(TIM3, DISABLE);
}

//��������ܽŵ�ƽ
static __INLINE void LF_SETBIT(uint8_t pin,uint8_t bit){
    if(bit == 1 ){
         LF_DOUT_SET_HIGH(pin); 
    }else if(bit == 0){
         LF_DOUT_SET_LOW(pin); 
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

//����λ��(���λ)
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

//Ϊ���ּ���,�����õ��Ǹ�λ��ǰ,��λ�ں�
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
    
    //����һ��ʱ�����Ƶ����״̬
    drv_MCBuff_pushPhrase(PHRASE_ON,5);

    //��ʼλ (1 pharse)
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
    
    //����(4B)  (64+2 phase)
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

void LF_IRQHandler(void){
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET){       
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        
        if(m_phase_sof<m_phase_eof){
            LF_SETBIT(LF_DOUTN_PINNO,m_phase_buffer[m_phase_sof++]);
        }
        
        if(m_phase_sof>=m_phase_eof){
            hal_LF_stopTimer();
        }
    }
}

