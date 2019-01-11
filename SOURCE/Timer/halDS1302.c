#include <includes.h>

#ifdef PIGEON_V6

#include "halDS1302.h"
__inline void def_dsrst_set(UInt8 x)
{
	GPIOE_ODR_BIT(4)=x; 
}
__inline void def_dsio_set(UInt8 x)
{
    GPIOE_ODR_BIT(3)=x;
}

__inline void def_dsclk_set(UInt8 x)
{   
	GPIOE_ODR_BIT(2)=x;
    delay_us(1);
}

__inline UInt8 def_dsio_get(void){

	return GPIOE_IDR_BIT(3);
}

void hal_DS1302_config(void){
	GPIO_InitTypeDef  gpio_init; 

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    
	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
	//DS1302 CLK RESET
	gpio_init.GPIO_Pin  = GPIO_Pin_2|GPIO_Pin_4;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &gpio_init);
//	//DS1302 IO
	gpio_init.GPIO_Pin  = GPIO_Pin_3;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOE, &gpio_init);

	def_dsclk_set(0);
  	def_dsrst_set(0);
}

void hal_DS1302_write(UInt16 _cldata){
	UInt8 i;
	def_dsclk_set(0);
	def_dsrst_set(0);
	def_dsrst_set(1);
	delay_us(4);
	for(i=0;i<16;i++){
		if ((i>0) && ((_cldata&(1<<i))!=0))	 //第一位写0
			def_dsio_set(1);
		else
			def_dsio_set(0);
		def_dsclk_set(1);
		def_dsclk_set(0);	
	}	
	def_dsrst_set(0);
	delay_us(4);
}

UInt8 hal_DS1302_read(UInt8 _cldata){
	UInt8 i,j,k=0;
	def_dsclk_set(0);
	def_dsrst_set(0);
	def_dsrst_set(1);
	delay_us(4);

	for(i=0;i<8;i++){
		if ((i==0)||((_cldata&(0x1<<i))!=0)) //第一位写1
			def_dsio_set(1);
		else
			def_dsio_set(0);
		def_dsclk_set(1);
		def_dsclk_set(0);
	}
    
	for(j=0;j<8;j++){
		if (def_dsio_get()==1)
			k=k+(1<<j);
        def_dsclk_set(1);
		def_dsclk_set(0);
	}	
	def_dsrst_set(0);
	delay_us(4);
	return(k);
}


void hal_DS1302_lock(void){
	hal_DS1302_write(0x808e);
}

void hal_DS1302_unlock(void){
	hal_DS1302_write(0x008e);
}

void hal_DS1302_setTime(DATETIME *pTm){
    DATETIME dt;
    dt.Year  = BinToBcd(pTm->Year);
    dt.Month = BinToBcd(pTm->Month);
    dt.Day   = BinToBcd(pTm->Day);
    dt.Hour  = BinToBcd(pTm->Hour);
    dt.Minute= BinToBcd(pTm->Minute);
    dt.Second= BinToBcd(pTm->Second);

    hal_DS1302_unlock();
	hal_DS1302_write(0x80+dt.Second*0x100);
	hal_DS1302_write(0x82+dt.Minute*0x100);
	hal_DS1302_write(0x86+dt.Day*0x100);
	hal_DS1302_write(0x84+dt.Hour*0x100);
	hal_DS1302_write(0x88+dt.Month*0x100);
	hal_DS1302_write(0x8c+dt.Year*0x100);  
    hal_DS1302_lock();

}


bool hal_DS1302_getTime(DATETIME *pTm){
    DATETIME dt;
	dt.Year=hal_DS1302_read(0x8d);
	dt.Month=hal_DS1302_read(0x89);
	dt.Day=hal_DS1302_read(0x87);
	dt.Hour=hal_DS1302_read(0x85);
	dt.Minute=hal_DS1302_read(0x83);
    dt.Second=hal_DS1302_read(0x81);
	if(dt.Second==0){	//可能会出现进位错误,重读一次
    	dt.Year=hal_DS1302_read(0x8d);
    	dt.Month=hal_DS1302_read(0x89);
    	dt.Hour=hal_DS1302_read(0x85);
    	dt.Day=hal_DS1302_read(0x87);
    	dt.Minute=hal_DS1302_read(0x83);
       	dt.Second=hal_DS1302_read(0x81);
	}

    pTm->Year = BcdToBin(dt.Year);
    pTm->Month = BcdToBin(dt.Month);
    pTm->Day   = BcdToBin(dt.Day);
    pTm->Hour  = BcdToBin(dt.Hour);
    pTm->Minute= BcdToBin(dt.Minute);
    pTm->Second= BcdToBin(dt.Second);
    return checkTime(pTm);
}

#endif
