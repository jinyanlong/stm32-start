#ifndef __NRF_H
#define __NRF_H

#include "CommDriver.h"
#include "halNrf.h"
typedef struct _NRFUART_PACKET_PARSER{
    UInt8 len;
    UInt8 chanel;
    UInt8 nrfCLA;
    UInt8 nrfINS;
    UInt8* stream;
}NRFUART_PACKET_PARSER;

void drv_WaitNrf_Ack(FV_RESPONSE* fvRsp);
void drv_NrfComm_onEvent(void);
void drv_Nrf_sendData(UInt8* data,UInt16 len);
void drv_Nrf_sendTest(void);
#endif
