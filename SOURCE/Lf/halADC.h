#ifndef  __HALADC_H__
#define  __HALADC_H__

uint32_t hal_ADC_getValue(uint8_t ch);
void hal_ADC_config(void);
void hal_ADC_stop(void);
uint32_t hal_ADC_getAverageVal(uint8_t ch,uint8_t times);
#endif