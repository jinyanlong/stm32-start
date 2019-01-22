#ifndef __HALLED_001
#define __HALLED_001
#ifdef __LED

#define LED_MAXCOUNT  2

#define LED_1      0        
#define LED_2      1 


#define LED_RED     LED_1   
#define LED_BLUE    LED_2 

void hal_Led_config(void);
void hal_Led_on(UInt8 pinNo);
void hal_Led_off(UInt8 pinNo);
void hal_Led_toggle(UInt8 pinNo);
bool hal_Led_isON(UInt8 pinNo);

#endif

#endif

