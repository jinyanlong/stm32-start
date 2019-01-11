#ifndef __CDC_H_001
#define __CDC_H_001

#include "halLcd.h"
#include "font.h"
#include "stringRes.h"


//黑白屏一律认为白底黑字
#define COLOR_BLACK 1
#define COLOR_WHITE 2
#define COLOR_NONE  0
#define COLOR_XOR   0xFF

typedef struct _RECT{
    UInt32 left;
    UInt32 top;
    UInt32 right;
    UInt32 bottom;
}RECT;
typedef struct _POINT{
    UInt32 x;
    UInt32 y;
}POINT;

typedef UInt8 COLOR;
struct _CDC;
typedef struct _FONTHANDLER{
    const UInt8* (*getBitmap)(int);
    UInt8 (*getWidth)(int ch);
    UInt8 (*draw)(struct _CDC* pCDC,int xpoint,int ypoint,int ch);
}FONTHANDLER;

typedef struct _CDC{
    COLOR color;
    const FONTHANDLER* font;
    UInt8 (*lcdBuff)[LCD_X_MAX+1];
}CDC;

COLOR def_CDC_getPoint(UInt8 (*pLcdBuff)[LCD_X_MAX+1],LCDSIZETYPE xpiont,LCDSIZETYPE ypoint);
void def_CDC_setPoint(UInt8 (*pLcdBuff)[LCD_X_MAX+1],LCDSIZETYPE xpiont,LCDSIZETYPE ypoint,COLOR color);

int WSTRING_WIDTH(const FONTHANDLER* font,wstring strText);
int STRING_WIDTH(const FONTHANDLER* font,string strText);

UInt8 fns_FontSmall_draw(CDC* pCDC,int xpoint,int ypoint,int ch);// 固定宽度的小字体
UInt8 fns_FontNormal_draw(CDC* pCDC,int xpoint,int ypoint,int ch);// 可变宽度的正常字体
void fns_CDC_setColor(CDC* pCDC,COLOR color);
void fns_CDC_setFont(CDC* pCDC,FONTHANDLER* font);

void fns_CDC_drawPoint(CDC* pCDC,UInt32 xpiont,UInt32 ypoint);
void fns_CDC_drawLine(CDC* pCDC,UInt32 x1,UInt32 y1,UInt32 x2,UInt32 y2);

void fns_CDC_drawRect(CDC* pCDC,UInt32 x1,UInt32 y1,UInt32 x2,UInt32 y2);
void fns_CDC_fillRect(CDC* pCDC,UInt32 x1,UInt32 y1,UInt32 x2,UInt32 y2);
//在某一行上绘制
void fns_CDC_drawAscFont(CDC* pCDC,UInt32 xpoint,UInt32 ypoint,const UInt8* fontLib);
void fns_CDC_drawUTFFont(CDC* pCDC,UInt32 xpoint,UInt32 ypoint,const UInt8* fontLib);
UInt32 fns_CDC_drawChar(CDC* pCDC,UInt32 xpoint,UInt32 ypoint,char ch);
UInt32 fns_CDC_drawWChar(CDC* pCDC,UInt32 xpoint,UInt32 ypoint,wchar_t coding);
UInt32 fns_CDC_drawString(CDC* pCDC,UInt32 xpoint,UInt32 ypoint,string strText);
UInt32 fns_CDC_drawWString(CDC* pCDC,UInt32 xpoint,UInt32 ypoint,wstring strText);

UInt32 fns_CDC_drawTime(CDC* pCDC,UInt32 x,UInt32 y,DATETIME* pTime);
UInt32 fns_CDC_drawDate(CDC* pCDC,UInt32 x,UInt32 y,DATETIME* pTime);
UInt32 fns_CDC_drawDateTime(CDC* pCDC,UInt32 x,UInt32 y,DATETIME* pTime);
UInt32 fns_CDC_drawHexs(CDC* pCDC,UInt32 x,UInt32 y,UInt8* pHex,UInt8 nCount);
UInt32 fns_CDC_drawHex(CDC* pCDC,UInt32 x,UInt32 y,UInt8 nHexVal);
void  fns_CDC_clear(CDC* pCDC,UInt8 bMask);
void  fns_CDC_copyToLCD(CDC* pCDC,RECT* pRect);
void  fns_CDC_copyFromLCD(CDC* pCDC,RECT* pRect);

void fns_CDC_drawByte(CDC* pCDC,LCDSIZETYPE xpoint,LCDSIZETYPE ypoint,UInt8 bytVal);
void fns_CDC_drawIcon16(CDC* pCDC,LCDSIZETYPE xpoint,LCDSIZETYPE ypoint,UInt8 iconIndex);
void fns_CDC_drawIcon24(CDC* pCDC,UInt8 xpoint,UInt8 ypoint,UInt8 iconIndex);
#endif

