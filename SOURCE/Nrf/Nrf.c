#include  <includes.h>
#include  "SFLib.h"
#include  "halCPU.h"
#include  "halUart.h"
#include  "Nrf.h"
#include  "ByteStream.h"
#include "Led.h"
#include "Func_Common.h"
#include "CommDriver.h"

#define NRFCOMM_TEST    0x80
#define NRFCOMM_PROTOCOL 0x82
extern NrfUartPacketHandler _nrfCommUart;

UInt8 drv_Nrf_sendProfix(UInt8 crc8){
    const UInt8 prefix[]={NRFCOMM_PROTOCOL,0x00,0x01,0x00};
    drv_Comm_sendBytes(_nrfCommUart.pUart,prefix,4);
        
    return crc8_byte_calc(crc8,prefix,4);
}


void drv_Nrf_sendData(UInt8* data,UInt16 len){
    UInt8 crc8;    
    hal_UART_sendByte(_nrfCommUart.pUart,CHAR_STB); 
    crc8=drv_Nrf_sendProfix(CRC8_PRESET);
    crc8=crc8_byte_calc(crc8,data,len);
    drv_Comm_sendBytes(_nrfCommUart.pUart,data,len);
    drv_Comm_sendByte(_nrfCommUart.pUart,crc8);
    hal_UART_sendByte(_nrfCommUart.pUart,CHAR_ETB);
}

void drv_Nrf_sendTest(void){
    UInt8 crc8;   
    const UInt8 prefix[4]={NRFCOMM_TEST,0x00,0x01,0x00};
    
    hal_UART_sendByte(_nrfCommUart.pUart,CHAR_STB);
    crc8=crc8_byte_calc(CRC8_PRESET,prefix,4);
    drv_Comm_sendBytes(_nrfCommUart.pUart,prefix,4);
    drv_Comm_sendByte(_nrfCommUart.pUart,crc8);
    hal_UART_sendByte(_nrfCommUart.pUart,CHAR_ETB);
}
