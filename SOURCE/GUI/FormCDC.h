#ifndef __FORMCDC_001
#define __FORMCDC_001

#include "CDC.h"

#define SCREEN_CLIENT_LEFT 		0
#define SCREEN_CLIENT_TOP 		0
#define SCREEN_CLIENT_RIGHT		LCD_X_MAX
#define SCREEN_CLIENT_BOTTOM 	LCD_Y_MAX
#define SCREEN_MAXLINE          3
#define SCREEN_MAXCOLUMN	    32

#define SCREEN_PAGE0 0x00           //��0����ʾͼ��(width=8),��1,2����ʾ����(width=12),�޼��
#define SCREEN_PAGE1 0x100          //��0����ʾ����(width=4),��1,2����ʾ����(width=12),���2

int WSTRING_XCENTER(const FONTHANDLER* font,wstring strText);
int STRING_XCENTER(const FONTHANDLER* font,string strText);

void drv_FormCDC_reset(CDC* pCDC);
void drv_FormCDC_clearLine(CDC* pCDC,UInt32 nRow);
void drv_FormCDC_xorLine(CDC* pCDC,UInt32 nRow);
void drv_FormCDC_xorLineEx(CDC* pCDC,UInt32 nRow,int margin,int height,UInt32 xStart,UInt32 xEnd);

void drv_FormCDC_drawChar(CDC* pCDC,UInt32 nRow,UInt32 x,char ch);
void drv_FormCDC_drawWChar(CDC* pCDC,UInt32 nRow,UInt32 x,wchar_t ch);
UInt32 drv_FormCDC_drawString(CDC* pCDC,UInt32 nRow,UInt32 x,string strText);
void drv_FormCDC_drawStringCenter(CDC* pCDC,UInt32 nRow,string strText);
UInt32 drv_FormCDC_drawWString(CDC* pCDC,UInt32 nRow,UInt32 x,wstring strText);
void drv_FormCDC_drawWStringCenter(CDC* pCDC,UInt32 nRow,wstring strText);
//void drv_FormCDC_drawId(CDC* pCDC,UInt32 nRow,UInt32 x,UInt32 sId);
//void drv_FormCDC_drawIdCenter(CDC* pCDC,UInt32 nRow,UInt32 sId);
void drv_FormCDC_drawIcon16(CDC* pCDC,UInt32 nRow,UInt32 x,UInt8 iconIndex);
void drv_FormCDC_drawIcon24(CDC* pCDC,UInt32 nRow,UInt32 x,UInt8 iconIndex);

#endif

