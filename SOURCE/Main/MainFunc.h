#ifndef _MAINFUNC_H
#define _MAINFUNC_H

void hook_Keyboard_onPress(UInt32 keyVal,UInt32 keyState);
void drv_System_tick(UInt32 nowTick);
void drv_CPU_reset(int reason);

#endif

