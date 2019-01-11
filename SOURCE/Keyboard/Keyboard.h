#ifndef __KEYBOARD_H_0001
#define __KEYBOARD_H_0001

#include  "halKeyBoard.h"

//键盘状态
#define KEYSTATE_NULL			0x00
#define KEYSTATE_READY			0x01
#define KEYSTATE_NEW			0x02
#define KEYSTATE_REPEAT			0x80

//以ms为单位
#define KEYDEALY_VALID			10
#define KEYDEALY_FIRSTREPEAT	800
#define KEYDEALY_NORMALREPEAT   400



typedef void (*HOOK_KEYBOARD_ONPRESS)(UInt32 keyVal,UInt32 keyState);

void drv_Keyboard_init(HOOK_KEYBOARD_ONPRESS pCallback);
void drv_Keyboard_tick(UInt32 now);
void fns_Keyboard_enable(void);
void fns_Keyboard_disable(void);

#endif
