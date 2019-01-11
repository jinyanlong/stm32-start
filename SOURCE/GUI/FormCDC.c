#include <includes.h>
#include "font.h"
#include "FormCDC.h"

//FormCDC按行处理
const int ROW_Y[][4]={{0,8,20,32},{0,4,18,32}};
int WSTRING_XCENTER(const FONTHANDLER* font,wstring strText){
    int nWidth=WSTRING_WIDTH(font,strText);
	if( nWidth >=LCD_X_MAX)
		return 0;
	return (LCD_X_MAX+1-nWidth)/2;
}

int STRING_XCENTER(const FONTHANDLER* font,string strText){
    int nWidth=STRING_WIDTH(font,strText);
	if( nWidth >=LCD_X_MAX)
		return 0;
	return (LCD_X_MAX+1-nWidth)/2;
}
void drv_FormCDC_reset(CDC* pCDC){
    fns_CDC_clear(pCDC,0);
}

void drv_FormCDC_clearLine(CDC* pCDC,UInt32 nRow){
    pCDC->color=COLOR_WHITE;
    fns_CDC_fillRect(pCDC,0,ROW_Y[nRow>>8][nRow&0xFF],SCREEN_CLIENT_RIGHT,ROW_Y[nRow>>8][(nRow&0xFF)+1]);
    pCDC->color=COLOR_BLACK;
}
void drv_FormCDC_xorLine(CDC* pCDC,UInt32 nRow){
    pCDC->color=COLOR_XOR;
	fns_CDC_fillRect(pCDC,0,ROW_Y[nRow>>8][nRow&0xFF],SCREEN_CLIENT_RIGHT,ROW_Y[nRow>>8][(nRow&0xFF)+1]);
    pCDC->color=COLOR_BLACK;
}

void drv_FormCDC_xorLineEx(CDC* pCDC,UInt32 nRow,int margin,int height,UInt32 xStart,UInt32 xEnd){
	UInt32 y0;
	y0=ROW_Y[nRow>>8][nRow&0xFF];
	if(y0<margin){
		y0=0;
	}else{
		y0-=margin;
	}
    pCDC->color=COLOR_XOR;
	fns_CDC_fillRect(pCDC, xStart,y0,xEnd,ROW_Y[nRow>>8][nRow&0xFF]+height+margin);
    pCDC->color=COLOR_BLACK;
}


void drv_FormCDC_drawChar(CDC* pCDC,UInt32 nRow,UInt32 x,char ch){
    fns_CDC_drawChar(pCDC,x,ROW_Y[nRow>>8][nRow&0xFF],ch);
}
void drv_FormCDC_drawWChar(CDC* pCDC,UInt32 nRow,UInt32 x,wchar_t ch){
    fns_CDC_drawWChar(pCDC,x,ROW_Y[nRow>>8][nRow&0xFF],ch);
}

UInt32 drv_FormCDC_drawString(CDC* pCDC,UInt32 nRow,UInt32 x,string strText){
    return fns_CDC_drawString(pCDC,x,ROW_Y[nRow>>8][nRow&0xFF],strText);
}
void drv_FormCDC_drawStringCenter(CDC* pCDC,UInt32 nRow,string strText){
	fns_CDC_drawString(pCDC,STRING_XCENTER(pCDC->font,strText),ROW_Y[nRow>>8][nRow&0xFF],strText);
}

UInt32 drv_FormCDC_drawWString(CDC* pCDC,UInt32 nRow,UInt32 x,wstring strText){
	return fns_CDC_drawWString(pCDC,x,ROW_Y[nRow>>8][nRow&0xFF],strText);
}
void drv_FormCDC_drawWStringCenter(CDC* pCDC,UInt32 nRow,wstring strText){
	fns_CDC_drawWString(pCDC,WSTRING_XCENTER(pCDC->font,strText),ROW_Y[nRow>>8][nRow&0xFF],strText);
}

void drv_FormCDC_drawIcon16(CDC* pCDC,UInt32 nRow,UInt32 x,UInt8 iconIndex){
    fns_CDC_drawIcon16(pCDC,x,ROW_Y[nRow>>8][nRow&0xFF],iconIndex);
}
void drv_FormCDC_drawIcon24(CDC* pCDC,UInt32 nRow,UInt32 x,UInt8 iconIndex){
    fns_CDC_drawIcon24(pCDC,x,ROW_Y[nRow>>8][nRow&0xFF],iconIndex);
}

//void drv_FormCDC_drawId(CDC* pCDC,UInt32 nRow,UInt32 nCol,UInt32 sId){
//	drv_FormCDC_drawWString(pCDC,nRow,nCol,LoadWString(sId));
//}
//void drv_FormCDC_drawIdCenter(CDC* pCDC,UInt32 nRow,UInt32 sId){
//    drv_FormCDC_drawWStringCenter(pCDC,nRow,LoadWString(sId));
//}
