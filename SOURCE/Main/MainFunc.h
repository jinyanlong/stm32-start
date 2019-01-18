#ifndef _MAINFUNC_H
#define _MAINFUNC_H

#include "MessageLoop.h"
void hook_Keyboard_onPress(UInt32 keyVal,UInt32 keyState);
void drv_System_tick(UInt32 nowTick);
void drv_CPU_reset(int reason);
void app_Exception_onRaise(Exception* exception);
void app_GUI_onBeforeMsg(MESSAGE* pMsg);
void app_GUI_onAfterMsg(MESSAGE* pMsg);
void app_GUI_showSplash(void);
#endif

