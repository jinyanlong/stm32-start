#ifndef __HALNRF_H
#define __HALNRF_H
#include "CommDriver.h"
void hal_Nrf_config(void);
void hal_Nrf_powerOff(void);
void hal_NRF_reset(void);
void drv_nrfComm_startRx(NrfUartPacketHandler* pUartHandler);

#endif
