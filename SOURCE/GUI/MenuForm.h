#ifndef _MENUFORM_H
#define _MENUFORM_H

#include "Form.h"
#define FORM_MAXLINE 2

struct _MENUFORM;
typedef struct _MENUITEM{
    int titleId;
    void (*onItem)(struct _MENUFORM*);
    void (*onShow)(struct _MENUFORM*,CDC*,const struct _MENUITEM*,int rowIndex);
} MENUITEM;


typedef struct _MENUFORM{
	FORM_HANDLERS const * handlers;
	struct _BASEFORM* owner;
	int timeTick;   //单位:ms,每当<=0时,触发一次时钟事件

	MENUITEM const * menuItems;
    int itemCount;
    int firstIndex;
	int curIndex;
} MENUFORM;


void drv_MenuForm_adjustIndex(MENUFORM* pThis,int linecount);
void drv_MenuForm_showArrow(MENUFORM* pThis,CDC* pCDC);
void drv_MenuForm_onChar(BASEFORM* pThis,UInt32 keyVal);
//默认的菜单条显示方式,菜单必须少于9条
void drv_MenuForm_showItem(CDC* pCDC,wstring pTitle,int firstIndex,int rowIndex);
void drv_MenuForm_onShowItem(MENUFORM* pThis,CDC* pCDC,const MENUITEM* pItem,int rowIndex);
void drv_MenuForm_onShow(BASEFORM* pThis,CDC* pCDC);
#endif
