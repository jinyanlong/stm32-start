#include  <includes.h>
#include  "SFLib.h"
#include  "halCPU.h"
#include  "halUart.h"
#include  "Nrf.h"
#include  "ByteStream.h"
#include "Led.h"
#include "Func_Common.h"
#include "CommDriver.h"

#define NRFCOMM_PROTOCOL 0X82
extern NrfUartPacketHandler _nrfCommUart;

UInt8 drv_Nrf_sendProfix(UInt8 crc8){
    UInt8 prefix[]={NRFCOMM_PROTOCOL,0X00,0X01,0X00};
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
