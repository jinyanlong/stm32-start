#ifndef __HALKEYBOARD_H_0001
#define __HALKEYBOARD_H_0001

#define KEY_CONTROL_COUNT    2

#define KEYVALUE_NULL		0x00
 
#define KEYVALUE_CANCEL		0x01
#define KEYVALUE_OK		0x02

//pb5-key1|pb14-key2   press-0
#define KEY1_PIN		GPIO_Pin_5
#define KEY2_PIN		GPIO_Pin_14

UInt16 hal_Keyboard_get(void);
void hal_Keyboard_config(void);
#endif






