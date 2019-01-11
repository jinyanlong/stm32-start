#ifndef __hal_LED_H
#define __hal_LED_H

#define LED_ON     0
#define LED_OFF    1

#define LED_1_PIN    9
#define LED_2_PIN    15

#define LED_1    0
#define LED_2    1

#define LED_COUNT  2

void hal_Led_config(void);
void hal_Led_set(UInt8 ledNo,UInt8 val);
void hal_Led_on(UInt8 ledNo);
void hal_Led_off(UInt8 ledNo);

#endif
