#include <includes.h>
#include "halLcd.h"
#include "halI2C.h"

#define LCD_ADDR			0x78	//从机地址，默认值
#define LCD_CTL_CON		    (1<<7)	//表示后面两个字节是数据字节和另外的控制字节
#define LCD_CTL_END		    (0<<7)	//表示最后一个控制字，后面跟液晶显示数据
#define LCD_CTL_CMD			(0<<6)	//控制字后面跟随的一个数据字节当做指令
#define LCD_CTL_RAM			(1<<6)	//控制字后面跟随的一个数据字节RAM显示

void hal_Lcd_config(void){
    GPIO_InitTypeDef  gpio_init;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
    
    //PB3是JTDO，PB4是JNTRST
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	// 改变指定管脚的映射 GPIO_Remap_SWJ_Disable SWJ 完全禁用（JTAG+SW-DP）
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
	// 改变指定管脚的映射 GPIO_Remap_SWJ_JTAGDisable ，JTAG-DP 禁用 + SW-DP 使能
    
    //RESET
	GPIOC_ODR_BIT(10)=0;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin =GPIO_Pin_10;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &gpio_init);
	
	hal_I2C_config();
}

void hal_Lcd_reset(void){
	GPIOC_ODR_BIT(10)=0;
	delay_us(20);
	GPIOC_ODR_BIT(10)=1;
	delay_us(20000);
}


bool hal_Lcd_cmdStart(void){
	bool ret=true;
	hal_I2C_start();
	
	if(!hal_I2C_send(LCD_ADDR)){//slave address
		ret=false;
	}
	return ret;	
}
bool hal_Lcd_cmdCtlB(void){
	bool ret=true;
	if(!hal_I2C_send(LCD_CTL_CMD|LCD_CTL_CON)){//control byte
		ret=false;
	}
	return ret;	
}
bool hal_Lcd_SRAMCtlB(void){
	bool ret=true;
	if(!hal_I2C_send(LCD_CTL_END|LCD_CTL_RAM)){//control byte
		ret=false;
	}
	return ret;	
}
bool hal_Lcd_cmdByte(UInt8 cmdData){
	bool ret=true;
	if(!hal_I2C_send(cmdData)){//command
		ret=false;
	}
	return ret;
}
bool hal_Lcd_SRAMByte(UInt8 ramData){
	bool ret=true;
	if(!hal_I2C_send(ramData)){//command
		ret=false;
	}
	return ret;
}
void hal_Lcd_cmdEnd(void){
	hal_I2C_stop();
}

bool hal_Lcd_writeCmd(UInt8 cmdData){
	bool ret=true;
	
	hal_Lcd_cmdStart();
	hal_Lcd_cmdCtlB();
	hal_Lcd_cmdByte(cmdData);
	hal_Lcd_cmdEnd();
	return ret;
}

void  hal_Lcd_clear(UInt8 Mask){
	UInt8 i,j;
	
	for(i=0;i<=LCD_Y_MAXPAGE;i++){
		hal_Lcd_cmdStart();
		hal_Lcd_cmdCtlB();
		hal_Lcd_cmdByte(0xB0+i);//页
		hal_Lcd_cmdCtlB();
		hal_Lcd_cmdByte(0x00);//列低地址
		hal_Lcd_cmdCtlB();
		hal_Lcd_cmdByte(0x10);//列高地址
		
		hal_Lcd_SRAMCtlB();
        for (j=0;j<=LCD_X_MAX;j++){
            hal_Lcd_SRAMByte(Mask);
		}
    }
}

void hal_Lcd_open(void){
    hal_I2C_init();
    hal_Lcd_reset();
	hal_Lcd_writeCmd(0xAE); //Set Display Off
	hal_Lcd_writeCmd(0xD5); //Display divide ratio/osc. freq. mode
	hal_Lcd_writeCmd(0x91);
	hal_Lcd_writeCmd(0xA8); //Multiplex ration mode:63
	hal_Lcd_writeCmd(0x1F);
	hal_Lcd_writeCmd(0xD3); //Set Display Offset
	hal_Lcd_writeCmd(0x00);
	hal_Lcd_writeCmd(0x40); //Set Display Start Line
	hal_Lcd_writeCmd(0xAD); //DC-DC Control Mode Set
	hal_Lcd_writeCmd(0x8B); //DC-DC ON/OFF Mode Set
	hal_Lcd_writeCmd(0x32); //Set Pump voltage value
	hal_Lcd_writeCmd(0xA1); //Segment Remap
	hal_Lcd_writeCmd(0xC8); //Set COM Output Scan Direction
	hal_Lcd_writeCmd(0xDA); //Common pads hardware: alternative
	hal_Lcd_writeCmd(0x02);
	hal_Lcd_writeCmd(0x81); //Contrast control
	hal_Lcd_writeCmd(0x33);
	hal_Lcd_writeCmd(0xD9); //Set pre-charge period
	hal_Lcd_writeCmd(0x1F);
	hal_Lcd_writeCmd(0xDB); //VCOM deselect level mode
	hal_Lcd_writeCmd(0x40);
	hal_Lcd_writeCmd(0xA4); //Set Entire Display On/Off
	hal_Lcd_writeCmd(0xA6); //Set Normal Display
    
    hal_Lcd_clear(0);
    hal_Lcd_writeCmd(0xAF); //Set Display On
    
}

void hal_Lcd_lightOn(void){
    hal_Lcd_writeCmd(0xAF); //Set Display On
}
void hal_Lcd_lightOff(void){
    hal_Lcd_writeCmd(0xAE);
}


void hal_Lcd_flushSetting_(void){
    hal_I2C_init();
    //hal_Lcd_reset();
	//hal_Lcd_writeCmd(0xAE); //Set Display Off
//	hal_Lcd_writeCmd(0xD5); //Display divide ratio/osc. freq. mode
//	hal_Lcd_writeCmd(0x91);
//	hal_Lcd_writeCmd(0xA8); //Multiplex ration mode:63
//	hal_Lcd_writeCmd(0x1F);
//	hal_Lcd_writeCmd(0xD3); //Set Display Offset
	hal_Lcd_writeCmd(0x00);
	hal_Lcd_writeCmd(0x40); //Set Display Start Line
	hal_Lcd_writeCmd(0xAD); //DC-DC Control Mode Set
	hal_Lcd_writeCmd(0x8B); //DC-DC ON/OFF Mode Set
	hal_Lcd_writeCmd(0x32); //Set Pump voltage value
	hal_Lcd_writeCmd(0xA1); //Segment Remap
	hal_Lcd_writeCmd(0xC8); //Set COM Output Scan Direction
	hal_Lcd_writeCmd(0xDA); //Common pads hardware: alternative
	hal_Lcd_writeCmd(0x02);
	hal_Lcd_writeCmd(0x81); //Contrast control
	hal_Lcd_writeCmd(0x33);
	hal_Lcd_writeCmd(0xD9); //Set pre-charge period
	hal_Lcd_writeCmd(0x1F);
	hal_Lcd_writeCmd(0xDB); //VCOM deselect level mode
	hal_Lcd_writeCmd(0x40);
//	hal_Lcd_writeCmd(0xA4); //Set Entire Display On/Off
//	hal_Lcd_writeCmd(0xA6); //Set Normal Display
    
    //hal_Lcd_clear(0);
    hal_Lcd_writeCmd(0xAF); //Set Display On
    
}


//更新显存到LCD 
//bool hal_Lcd_flush_(void* pBuff){
//	UInt8 i,j;
//    hal_Lcd_flushSetting_();
//	for(i=0;i<=LCD_Y_MAXPAGE;i++){
//		if(!hal_Lcd_cmdStart()) return false;
//		if(!hal_Lcd_cmdCtlB()) return false;
//		if(!hal_Lcd_cmdByte(0xB0+i)) return false;//页
//		if(!hal_Lcd_cmdCtlB()) return false;
//		if(!hal_Lcd_cmdByte(0x00)) return false;//列低地址
//		if(!hal_Lcd_cmdCtlB()) return false;
//		if(!hal_Lcd_cmdByte(0x10)) return false;//列高地址
//		
//		if(!hal_Lcd_SRAMCtlB()) return false;
//        for (j=0;j<=LCD_X_MAX;j++){
//            if(!hal_Lcd_SRAMByte(((UInt8*)pBuff)[i*(LCD_X_MAX+1)+(LCD_X_MAX-j)])) return false;
//		}       
//        hal_Lcd_cmdEnd();
//	}
//    return true;
//}

bool hal_Lcd_flush_(void* pBuff){
	UInt8 i,j;
	for(i=0;i<=LCD_Y_MAXPAGE;i++){
		if(!hal_Lcd_cmdStart()) return false;
		if(!hal_Lcd_cmdCtlB()) return false;
		if(!hal_Lcd_cmdByte(0xB0+i)) return false;
		if(!hal_Lcd_cmdCtlB()) return false;
		if(!hal_Lcd_cmdByte(0x00)) return false;
		if(!hal_Lcd_cmdCtlB()) return false;
		if(!hal_Lcd_cmdByte(0x10)) return false;
		
		if(!hal_Lcd_SRAMCtlB()) return false;
        for (j=0;j<=LCD_X_MAX;j++){
            if(!hal_Lcd_SRAMByte(((UInt8*)pBuff)[i*(LCD_X_MAX+1)+(LCD_X_MAX-j)])) return false;
		}
        hal_Lcd_cmdEnd();
	}
    return true;
}


bool hal_Lcd_flush(void* pBuff){
    bool ret;
    ret=hal_Lcd_flush_(pBuff);
    if(!ret){
        hal_Lcd_cmdEnd();
        return false;
    }else{
        return true;
    }
}

