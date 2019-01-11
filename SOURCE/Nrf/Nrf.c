#include  <includes.h>
#include  "SFLib.h"
#include  "halCPU.h"
#include  "halUart.h"
#include  "Nrf.h"
#include  "ByteStream.h"
#include "Led.h"
#include "Func_Common.h"
#include "CommDriver.h"

extern NrfUartPacketHandler _nrfCommUart;


void drv_Nrf_sendData(UInt8* data,UInt16 len){
    UInt8 crc8;
    crc8=crc8_byte_calc(CRC8_PRESET,data,len);
    
    hal_UART_sendByte(_nrfCommUart.pUart,CHAR_STB); 
    drv_Comm_sendBytes(_nrfCommUart.pUart,data,len);
    drv_Comm_sendByte(_nrfCommUart.pUart,crc8);
    hal_UART_sendByte(_nrfCommUart.pUart,CHAR_ETB);
}
