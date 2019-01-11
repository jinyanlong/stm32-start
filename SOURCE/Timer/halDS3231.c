#include <includes.h>

#ifdef PIGEON_V7

#include "halDS3231.h"
#define I2C_ADDR_DS3231     0xD0
#define I2C_FLAG_WRITE  0
#define I2C_FLAG_READ   1

#define DS3231_RST          GPIOE_ODR_BIT(4)
#define DS3231_DSIO_OUT     GPIOE_ODR_BIT(3)
#define DS3231_DSIO_IN      GPIOE_IDR_BIT(3)
#define DS3231_CLK          GPIOE_ODR_BIT(2)


#define def_scl_delay()  delay_us(2)
#define def_sda_delay()
#define def_byte_delay()

static void def_scl_1(void){
    DS3231_CLK=1;
	def_scl_delay();
}

static void def_scl_0(void){
    DS3231_CLK=0;
	def_scl_delay();
}

static void def_sda_1(void){
    DS3231_DSIO_OUT=1;
    def_sda_delay();
}

static void def_sda_0(void){
    DS3231_DSIO_OUT=0;
	def_sda_delay();
}

static bool def_sda_get(void){
	if(DS3231_DSIO_IN){
		return true;
	}else{
		return false;
	}
}

void hal_I2C_init(void){ // 初始化
	def_sda_1();
	def_scl_1();
}

//SCL为高时，拉低SDA表示启动
void hal_I2C_start(void){
	def_sda_1();
	def_scl_1();
	
	def_sda_0();
	def_scl_0();
}

//SCL为高时，拉高SDA表示停止
void hal_I2C_stop(void){
	def_sda_0();
	def_scl_1();
	def_sda_1();
}

void hal_I2C_ack(void){
	def_sda_0();
	def_scl_1();
	def_scl_0();
	def_sda_1();
}	
void hal_I2C_noack(void){
	def_sda_1();
	def_scl_1();
	def_scl_0();
}





bool hal_I2C_send(unsigned char bytData){
	bool ret;
	unsigned char i;
	for(i=0;i<8;i++){
	    if( bytData & (0x80>>i) ){
			def_sda_1();
		}else{
			def_sda_0();
		}
		def_scl_1();
		def_scl_0();
	}
    
	//查看ACK应答
	def_sda_1();
	def_scl_1();
	if(def_sda_get()){
		ret= false;		//读到为低电平表示为ACK应答
	}else{
		ret= true;
	}
	def_scl_0();
        
	return ret;
}

unsigned char hal_I2C_receive(void){
	unsigned char i,bytData;

	bytData=0;
	def_sda_1();
	def_scl_1();
	for(i=0;i<8;i++){
		def_scl_1();
		if(def_sda_get()){
	        bytData|=(0x80>>i);
		}
		def_scl_0();
	}
	return bytData;
}

void hal_DS3231_config(void){
	GPIO_InitTypeDef  gpio_init;  
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);
	gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    
	//CLK
    DS3231_CLK=1;
	gpio_init.GPIO_Pin  = GPIO_Pin_2;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &gpio_init);
    
	//IO,RST
	DS3231_RST=1; //防止复位
    DS3231_DSIO_OUT=1;
    gpio_init.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOE, &gpio_init);
}


bool hal_DS3231_writeByte(UInt8 regAddr,UInt8 data){
	hal_I2C_start();
	
	if(!hal_I2C_send(I2C_ADDR_DS3231|I2C_FLAG_WRITE)){//slave address
		return false;
	}
    
    if(!hal_I2C_send(regAddr)){//slave address
		return false;
	}
    
    if(!hal_I2C_send(data)){//slave address
		return false;
	}
    
    hal_I2C_stop();
    
	return true;	
}

bool hal_DS3231_writeBytes(UInt8 regAddr,UInt8* buff,UInt32 count){
    UInt32 i;
    if(count==0) return false;
    
	hal_I2C_start();
	
	if(!hal_I2C_send(I2C_ADDR_DS3231|I2C_FLAG_WRITE)){//slave address
		return false;
	}
    
    if(!hal_I2C_send(regAddr)){//slave address
		return false;
	}
    for(i=0;i<count;i++){
        if(!hal_I2C_send(buff[i])){//slave address
            return false;
        }
    }
    
    hal_I2C_stop();
	return true;	
}

//UInt8 hal_DS3231_readByte(UInt8 regAddr){
//    UInt8 result;
//    hal_I2C_start();
//    
//	if(!hal_I2C_send(I2C_ADDR_DS3231|I2C_FLAG_WRITE)){//slave address
//		return false;
//	}
//    if(!hal_I2C_send(regAddr)){//slave address
//		return false;
//	}
//    
//    hal_I2C_start();
//    if(!hal_I2C_send(I2C_ADDR_DS3231|I2C_FLAG_READ)){//slave address
//		return false;
//	}
//    
//    result=hal_I2C_receive();
//    hal_I2C_noack();
//    
//    hal_I2C_stop();
//    return result;
//}

bool hal_DS3231_readBytes(UInt8 regAddr,UInt8* buff,UInt32 count){
    UInt32 i;
    if(count==0) return false;
    
    hal_I2C_start();
    
	if(!hal_I2C_send(I2C_ADDR_DS3231|I2C_FLAG_WRITE)){//slave address
		return false;
	}
    if(!hal_I2C_send(regAddr)){//slave address
		return false;
	}
    
    hal_I2C_start();
    if(!hal_I2C_send(I2C_ADDR_DS3231|I2C_FLAG_READ)){//slave address
		return false;
	}
    
    for(i=0;i<count-1;i++){
        buff[i]=hal_I2C_receive();
        hal_I2C_ack();
    }
    
    buff[count-1]=hal_I2C_receive();
    hal_I2C_noack();
    hal_I2C_stop();
    return true;
}

void hal_DS3231_init(void){
    hal_I2C_init();
}

bool hal_DS3231_setTime(DATETIME *pTm){
    UInt8 buff[7],buff2[7];
    
    //模式设置为24小时制
    buff[0x06] = BinToBcd(pTm->Year);
    buff[0x05] = BinToBcd(pTm->Month);
    buff[0x04] = BinToBcd(pTm->Day);
    buff[0x03] = 1;
    buff[0x02] = BinToBcd(pTm->Hour);
    buff[0x01] = BinToBcd(pTm->Minute);
    buff[0x00] = BinToBcd(pTm->Second);
    if( !hal_DS3231_writeBytes(0,buff,7) ) return false;
    
    //为防止进位错误, 再读一次
    if(!hal_DS3231_readBytes(0,buff2,7)) return false;
    if( memcmp(buff,buff2,7)){ //写入的数据不吻合,重新写一次
        if( !hal_DS3231_writeBytes(0,buff,7) ) return false;    
    }
    
    //确保晶振使能
    return hal_DS3231_writeByte(0x0E,0);
}





bool hal_DS3231_getTime(DATETIME *pTm){
    UInt8 buff[7];
    
    if(!hal_DS3231_readBytes(0,buff,7)) return false;
    if(buff[0]==0){
        //秒为0时,重读一次,防止进位错误
        if(!hal_DS3231_readBytes(0,buff,7)) return false;
    }
        
    
    pTm->Year = BcdToBin(buff[6]);
    pTm->Month= BcdToBin(buff[5]&0x01F);
    pTm->Day   = BcdToBin(buff[4]);
    if(buff[2]&0x40){  //12小时制
        if(buff[2]&0x20){
            pTm->Hour  = BcdToBin(buff[2] & 0x1F)+12;
        }else{
            pTm->Hour  = BcdToBin(buff[2] & 0x1F);
        }
    }else{
        pTm->Hour  = BcdToBin(buff[2] & 0x3F);
    }
    pTm->Minute= BcdToBin(buff[1]);
    pTm->Second= BcdToBin(buff[0]);
    
    return checkTime(pTm);
}

#define TEMPERATURE_INVALID_VALUE  -32768
short hal_DS3231_getTemperature(void){
    short temp;
    UInt8 status;

    if(!hal_DS3231_readBytes(0x0F,&status,1)){
        return TEMPERATURE_INVALID_VALUE;
    }
    if(!hal_DS3231_readBytes(0,(UInt8*)&temp,1)) return TEMPERATURE_INVALID_VALUE;
    if(!hal_DS3231_readBytes(0,((UInt8*)&temp)+1,0)) return TEMPERATURE_INVALID_VALUE;
    
    return TEMPERATURE_INVALID_VALUE;
}
#endif
