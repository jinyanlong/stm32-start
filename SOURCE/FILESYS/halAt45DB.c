#include "includes.h"
#include "halAT45DB.h"
#include "SFile.h"

#define AT45DB_TAGMASK     0xBD
#define AT45DB161_TAGVAL   0xAC

//Dataflash opcodes SPI Mode 3
#define OP_FlashPageRead           0xD2   // Main memory page read
#define OP_FlashToBuf1Transfer     0x53   // Main memory page to buffer 1 transfer
#define OP_Buf1Read                0xD4   // Buffer 1 read
#define OP_FlashToBuf2Transfer     0x55   // Main memory page to buffer 2 transfer
#define OP_Buf2Read                0xD6   // Buffer 2 read
#define OP_StatusReg               0xD7   // Status register
#define OP_AutoPageReWrBuf1        0x58   // Auto page rewrite through buffer 1
#define OP_AutoPageReWrBuf2        0x59   // Auto page rewrite through buffer 2
#define OP_FlashToBuf1Compare      0x60   // Main memory page to buffer 1 compare
#define OP_FlashToBuf2Compare      0x61   // Main memory page to buffer 2 compare
#define OP_ContArrayRead           0xE8   // Continuous Array Read (Note : Only A/B-parts supported)
#define OP_FlashProgBuf1           0x82   // Main memory page program through buffer 1
#define OP_Buf1ToFlashWE           0x83   // Buffer 1 to main memory page program with built-in erase
#define OP_Buf1Write               0x84   // Buffer 1 write
#define OP_FlashProgBuf2           0x85   // Main memory page program through buffer 2
#define OP_Buf2ToFlashWE           0x86   // Buffer 2 to main memory page program with built-in erase
#define OP_Buf2Write               0x87   // Buffer 2 write
#define OP_Buf1ToFlash             0x88   // Buffer 1 to main memory page program without built-in erase
#define OP_Buf2ToFlash             0x89   // Buffer 2 to main memory page program without built-in erase
#define OP_PageErase               0x81   // Page erase, added by Martin Thomas
#define OP_BlockErase              0x50
#define OP_SectorErase             0x7C

//////////////////////////////////SPI总线操作///////////////////////////////////////////////////////
#define def_spisck_delay()
//static void def_spisck_delay(void){ //25ns
//	volatile int i;
//	for(i=0;i<1;i++) __nop();	//似乎可以不延迟?
//}
static void def_spics_delay(void){//250ns
    volatile int i;
    for(i=0;i<5;i++) __nop();	//似乎可以不延迟?
}

void hal_AT45DB_configPin(void){
    GPIO_InitTypeDef  gpio_init;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
   
	//sck-pa4|si-pa5|cs-pa6|so-pa7
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
 
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
    gpio_init.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_Init(GPIOA, &gpio_init);
									
    gpio_init.GPIO_Speed=GPIO_Speed_50MHz;
    gpio_init.GPIO_Pin  = GPIO_Pin_7;
    gpio_init.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
    GPIO_Init(GPIOA, &gpio_init);
}

//sck-pa4|si-pa5|cs-pa6|so-pa7
static void hal_spics_1(){
    GPIOA_ODR_BIT(6)=1;
    def_spics_delay();
}
static void hal_spics_0(){
    GPIOA_ODR_BIT(6)=0;
    def_spics_delay();
}
static void hal_spisck_1(){
    GPIOA_ODR_BIT(4)=1;
    def_spisck_delay();
}
static void hal_spisck_0(){
    GPIOA_ODR_BIT(4)=0;
    def_spisck_delay();
}
static void hal_spimosi_1(){
    GPIOA_ODR_BIT(5)=1;
}
static void hal_spimosi_0(){
    GPIOA_ODR_BIT(5)=0;
}
static  bool hal_spimiso_get(){
    if(GPIOA->IDR & GPIO_Pin_7)
        return true;
    else
        return false;
}


static void hal_SPI_sendbyte(UInt8 bytVal){
	UInt8 i;

	for( i=0;i<8;i++){
		if(bytVal & (0x80>>i)){
			hal_spimosi_1();
		}else{
			hal_spimosi_0();
		}
		hal_spisck_1();
		hal_spisck_0();	
	}
}

static UInt8 hal_SPI_getbyte(void){
	UInt8 bytVal,i;

	bytVal=0;
	for(i=0;i<8;i++){
		hal_spisck_1();
		if( hal_spimiso_get() ){
			bytVal|=(0x80>>i);
		}
		hal_spisck_0();	
	}
	return bytVal;
}



////////////////////////////////////AT45DB/////////////////////////////////////////////////////////
UInt8 hal_AT45DB_getState(void){
	UInt8 status;
	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	//发送指令
	hal_SPI_sendbyte(0xD7);

	//接收
	status=hal_SPI_getbyte();

	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();

                                              // 读状态寄存器指令
	return status;
}


bool hal_AT45DB_config(void){
    hal_AT45DB_configPin();

	hal_spisck_1();
   	hal_spics_1();
	hal_spimosi_1();
	
	if( ( hal_AT45DB_getState() & AT45DB_TAGMASK)==0xAC ){
		return true;
	}else{
		return false;
	}
}

void hal_AT45DB_readPage(UInt16 uPage,UInt16 uAddr,void* pBuff, UInt16 bLen){
	UInt16 i;
	UInt32 nAddr3;
    nAddr3=(uPage<<10)+ uAddr;
    
	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	//发送指令
	hal_SPI_sendbyte(OP_FlashPageRead);
	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
	hal_SPI_sendbyte((UInt8)(nAddr3));
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);
    
	//接收	
	for(i=0;i<bLen;i++){
		((UInt8*)pBuff)[i]=hal_SPI_getbyte();
	}
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();

}

void hal_AT45DB_readBuff1(UInt16 uAddr,void  * pBuff,UInt16 nLen){
	UInt16  i;
	
	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	//发送指令
	hal_SPI_sendbyte(OP_Buf1Read);
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte((BYTE)(uAddr>>8));
	hal_SPI_sendbyte((BYTE)uAddr);
	hal_SPI_sendbyte(0);

	
	//接收			
	for(i=0;i<nLen;i++){
		((BYTE*)pBuff)[i]=hal_SPI_getbyte();
	}
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();
}


//位反写
void hal_AT45DB_writeBuff1N(UInt16 uAddr,const void* pBuff,UInt16 nLen){
	UInt16 i;
	
	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();

	
	//发送指令
	hal_SPI_sendbyte(OP_Buf1Write);
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte((UInt8)(uAddr>>8));
	hal_SPI_sendbyte((UInt8)uAddr);
	
	//发送数据
	for(i=0;i<nLen;i++){
		hal_SPI_sendbyte(~((UInt8*)pBuff)[i]);
	}
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();

	while( (hal_AT45DB_getState()&AT45DB_TAGMASK)!=0xAC );
}

void hal_AT45DB_writeBuff1(UInt16 uAddr,const void* pBuff,UInt16 nLen){
	UInt16 i;
	
	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();

	
	//发送指令
	hal_SPI_sendbyte(OP_Buf1Write);
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte((UInt8)(uAddr>>8));
	hal_SPI_sendbyte((UInt8)uAddr);
	
	//发送数据
	for(i=0;i<nLen;i++){
		hal_SPI_sendbyte(((UInt8*)pBuff)[i]);
	}
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();

	while( (hal_AT45DB_getState()&AT45DB_TAGMASK)!=0xAC );
}

void hal_AT45DB_flushBuff1(UInt16 uPage){
    UInt32 nAddr3;
    nAddr3=(uPage<<10);
    
    //启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	//发送指令
	hal_SPI_sendbyte(OP_Buf1ToFlashWE);
	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
	hal_SPI_sendbyte((UInt8)(nAddr3));
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();


	while( (hal_AT45DB_getState()&AT45DB_TAGMASK)!=0xAC );

}
void hal_AT45DB_loadBuff1(UInt16 uPage){
    UInt32 nAddr3;
    nAddr3=(uPage<<10);

	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	//发送指令
	hal_SPI_sendbyte(OP_FlashToBuf1Transfer);
	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
	hal_SPI_sendbyte((UInt8)(nAddr3));

	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();

	while( (hal_AT45DB_getState()&AT45DB_TAGMASK)!=0xAC );
}


/*
void hal_AT45DB_rewrite(UInt16 uPage){
    UInt32 nAddr3;
    nAddr3=(uPage<<10);

	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	//发送指令
	hal_SPI_sendbyte(OP_AutoPageReWrBuf2);
	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
	hal_SPI_sendbyte((UInt8)(nAddr3));
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();

	while( (hal_AT45DB_getState() & AT45DB_TAGMASK)!=AT45DB161_TAGVAL );
}*/


//-----------------------------------------
//Main Memory Page Program Through Buffer
//把buff1内容写到指定地址的flash中，以页为单位烧写flash
//入参：
//uPage要写入到的页地址（目的）, BuffAddr在buff内部的要写字节的起始地址（源）
//一定是页（此处以512/page算）
//12 page address bits (A20 - A9) 
// 9 buffer address bits (BFA8 - BFA0)
void hal_AT45DB_programBuff1(UInt16 uPage){
    UInt32 nAddr3;
    nAddr3=(uPage<<10);
    
    //启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	//发送指令
	hal_SPI_sendbyte(OP_Buf1ToFlash);
	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
	hal_SPI_sendbyte((UInt8)(nAddr3));
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();


	while( (hal_AT45DB_getState()&AT45DB_TAGMASK)!=0xAC );
}
//V6.68发现一个严重的BUG,at45db161的programbuff搞错
// void hal_AT45DB_programbuff1_(UInt16 uPage,UInt16 nBuffAddr){
//     UInt32 nAddr3;
// 		
//     nAddr3=(uPage<<10) + nBuffAddr;

// 	//启动SPI
//    	hal_spics_0();
//    	hal_spisck_0();
// 	

// 	hal_SPI_sendbyte(OP_FlashProgBuf1);	 	//发送指令0x82, Main memory page program through buffer 1
// 	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
// 	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
// 	hal_SPI_sendbyte((UInt8)nAddr3);
// 	
// 	
// 	//结束SPI
//    	hal_spisck_1();
//    	hal_spics_1();

// 	while( (hal_AT45DB_getState() & AT45DB_TAGMASK)!=AT45DB161_TAGVAL );
// }
// void hal_AT45DB_programBuff1(UInt16 uPage){
//     hal_AT45DB_programbuff1_(uPage,0);
// }

//buff2
void hal_AT45DB_programBuff2(UInt16 uPage){
    UInt32 nAddr3;
    nAddr3=(uPage<<10);
    
    //启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	//发送指令
	hal_SPI_sendbyte(OP_Buf2ToFlash);
	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
	hal_SPI_sendbyte((UInt8)(nAddr3));
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();


	while( (hal_AT45DB_getState()&AT45DB_TAGMASK)!=0xAC );
}

// bool hal_AT45DB_programbuff2_(UInt16 uPage,UInt16 nBuffAddr){
// 	UInt32 nAddr3;
// 	
// 	nAddr3=(uPage<<10) | nBuffAddr;
// 	//启动SPI
//    	hal_spics_0();
//    	hal_spisck_0();
// 	

// 	hal_SPI_sendbyte(OP_FlashProgBuf2);	 	//发送指令0x85, Main memory page program through buffer 2
// 	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
// 	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
// 	hal_SPI_sendbyte((UInt8)nAddr3);
// 	

// 	//结束SPI
//    	hal_spisck_1();
//    	hal_spics_1();

// 	while( (hal_AT45DB_getState() & AT45DB_TAGMASK)!=AT45DB161_TAGVAL );
// 	return true;
// }
// void hal_AT45DB_programBuff2(UInt16 uPage){
//     hal_AT45DB_programbuff2_(uPage,0);
// }

bool hal_AT45DB_checkPage(UInt16 uPage,UInt16 uAddr,const void* pDataBuff, UInt16 nlen ){
	UInt16 i;
    bool bRet=true;
    UInt32 nAddr3;
    nAddr3=(uPage<<10)+ uAddr;

	
	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	hal_SPI_sendbyte(OP_FlashPageRead);	//发送指令 // Main memory page read
	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
	hal_SPI_sendbyte((UInt8)nAddr3);
	hal_SPI_sendbyte(0);  //4dummy bytes
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);

	
	//接收数据并检查
	for(i=0;i<nlen;i++){
        if( ((UInt8*)pDataBuff)[i]!= hal_SPI_getbyte() ){
            bRet=false;
            break;
        }
	}
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();
    return bRet;
}

bool hal_AT45DB_checkPageN(UInt16 uPage,UInt16 uAddr,const void* pDataBuff, UInt16 nlen ){
	UInt16 i;
    bool bRet=true;
    UInt32 nAddr3;
    nAddr3=(uPage<<10)+ uAddr;

	
	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	hal_SPI_sendbyte(OP_FlashPageRead);	//发送指令 // Main memory page read
	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
	hal_SPI_sendbyte((UInt8)nAddr3);
	hal_SPI_sendbyte(0);  //4dummy bytes
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);

	
	//接收数据并检查
	for(i=0;i<nlen;i++){
        if( ((UInt8*)pDataBuff)[i]!= (UInt8)(~hal_SPI_getbyte()) ){
            bRet=false;
            break;
        }
	}
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();
    return bRet;
}

bool hal_AT45DB_checkValue(UInt16 uPage,UInt8 nValue){
	UInt16 i;
    bool bRet=true;
	UInt32 nAddr3;
    nAddr3=(uPage<<10);

	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	hal_SPI_sendbyte(OP_FlashPageRead);	//发送指令 // Main memory page read
	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
	hal_SPI_sendbyte((UInt8)nAddr3);
	hal_SPI_sendbyte(0);  //4dummy bytes
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);

	
	//接收数据并检查
	for(i=0;i<AT45DB_PAGE_SIZE;i++){
        if( nValue != hal_SPI_getbyte() ){
            bRet=false;
            break;
        }
	}
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();
    
    return bRet;
}
void hal_AT45DB_erasePage(UInt16 uPage){
	UInt32 nAddr3;
    nAddr3=(uPage<<10);

	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	hal_SPI_sendbyte(OP_PageErase);	 	
	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
	hal_SPI_sendbyte((UInt8)nAddr3);
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();
    
	while( (hal_AT45DB_getState() & AT45DB_TAGMASK)!=AT45DB161_TAGVAL );
}

void hal_AT45DB_eraseSector(UInt16 sector){
	UInt32 nAddr3;
	
	nAddr3=(sector<<18);
	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	

	hal_SPI_sendbyte(OP_SectorErase);	 	//opcode
	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
	hal_SPI_sendbyte((UInt8)nAddr3);
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();

    while( (hal_AT45DB_getState() & AT45DB_TAGMASK)!=AT45DB161_TAGVAL );
}

void hal_AT45DB_eraseBlock(UInt16 nBlock){
	UInt32 nAddr3;
	
	nAddr3=(nBlock<<13);
	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	hal_SPI_sendbyte(OP_BlockErase);	 	//opcode
	hal_SPI_sendbyte((UInt8)(nAddr3>>16));
	hal_SPI_sendbyte((UInt8)(nAddr3>>8));
	hal_SPI_sendbyte((UInt8)nAddr3);
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();

    while( (hal_AT45DB_getState() & AT45DB_TAGMASK)!=AT45DB161_TAGVAL );
}
void hal_AT45DB_fillBuff1(UInt8 nValue){
	UInt16  i;
	
	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();
	
	//发送指令
	hal_SPI_sendbyte(OP_Buf1Write);
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);
	
	//发送数据
	for(i=0;i<AT45DB_PAGE_SIZE;i++){
		hal_SPI_sendbyte(nValue);
	}
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();

	while( (hal_AT45DB_getState() & AT45DB_TAGMASK)!=AT45DB161_TAGVAL );
}

void hal_AT45DB_fillBuff2(UInt8 nValue){
	UInt16  i;
	
	//启动SPI
   	hal_spics_0();
   	hal_spisck_0();

	//发送指令
	hal_SPI_sendbyte(OP_Buf2Write);
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);
	hal_SPI_sendbyte(0);
	
	//发送数据
	for(i=0;i<AT45DB_PAGE_SIZE;i++){
		hal_SPI_sendbyte(nValue);
	}
	
	//结束SPI
   	hal_spisck_1();
   	hal_spics_1();

	while( (hal_AT45DB_getState() & AT45DB_TAGMASK)!=AT45DB161_TAGVAL );
}


//////////////////////////////////////////////////移植////////////////////////////////////////////
void hal_Flash_init(void){
    //特殊处理:
    //BUFFER1用作正常数据读写的缓冲通道
    hal_AT45DB_fillBuff1(0xFF);
    
    //BUFFER2用作DUMP操作(写0)    
    hal_AT45DB_fillBuff2(0);
}
void hal_Flash_readPage(UInt16 pageIndex,UInt16 uAddr,UInt8* pageBuff,UInt16 nSize){
    hal_AT45DB_readPage(pageIndex,uAddr,pageBuff,nSize);
}


UInt16 s_pageIndex;
void hal_Flash_readPageN(UInt16 pageIndex,UInt16 uAddr,UInt8* pageBuff,UInt16 nSize){
    hal_AT45DB_readPage(pageIndex,uAddr,pageBuff,nSize);
    memnor(pageBuff,nSize);  //执行一次取反操作,使得擦除后,数据初始值为0
}

void hal_Flash_dumpPage(UInt16 pageIndex){
    hal_AT45DB_programBuff2(pageIndex);
}



bool hal_Flash_writePage(UInt16 pageIndex,UInt8* pageBuff){
    hal_AT45DB_writeBuff1(0,pageBuff,AT45DB161_PAGE_SIZE); //反写,读出时也需要反读
    hal_AT45DB_programBuff1(pageIndex);
    
#ifdef __DEBUG
    return hal_AT45DB_checkPage(pageIndex,0,pageBuff,AT45DB161_PAGE_SIZE) ;
#else
    //简化:这里不做坏快检查(总是认为hal_Flash_writePage不会失败)
    return true;
#endif
}

bool hal_Flash_writePageN(UInt16 pageIndex,UInt8* pageBuff){
    hal_AT45DB_writeBuff1N(0,pageBuff,AT45DB161_PAGE_SIZE); //反写,读出时也需要反读
    hal_AT45DB_programBuff1(pageIndex);
#ifdef __DEBUG
    return hal_AT45DB_checkPageN(pageIndex,0,pageBuff,AT45DB161_PAGE_SIZE) ;
#else
    return true;
#endif
}

void hal_Flash_eraseSector(UInt16 sectorIndex){
    if(NAND_SECTOR_PAGECOUNT==AT45DB161_BLOCK_PAGECOUNT){
        hal_AT45DB_eraseBlock(sectorIndex);
    }else  if(NAND_SECTOR_PAGECOUNT==1){
        hal_AT45DB_erasePage(sectorIndex);
    }else{
        //配置异常
    }
}

bool hal_Flash_checkPage(UInt16 pageIndex,UInt8 value){
    return hal_AT45DB_checkValue(pageIndex,value);
}
bool hal_Flash_checkPageN(UInt16 pageIndex,UInt8 value){
    return hal_AT45DB_checkValue(pageIndex,~value);
}





/////////////////////////////////////////////////////////////////////////////////////

void hal_Flash_setFlag(UInt16 pageIndex,UInt32 flag){
    hal_AT45DB_fillBuff1(0xFF);
    hal_AT45DB_writeBuff1(0,&flag,4);
    hal_AT45DB_programBuff1(pageIndex);
}

UInt32 hal_Flash_getFlag(UInt16 pageIndex){
    UInt32 flag;
    hal_Flash_readPage(pageIndex,0,(UInt8*)&flag,4);
    return flag;
}


