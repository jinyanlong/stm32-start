#include <includes.h>
#include "CDC.h"
#include "font.h"

/////////////////////字体///////////////////////////
UInt8 fns_FontSmall_getFixedWidth(int wch){
    return FONT_SMALL_DIGITWIDTH;
}
//FIXED只用于显示字母数字
UInt8 fns_FontNormal_getFixedWidth(int wch){
    return FONT_NORMAL_DIGITWIDTH;
}

////小字体(仅支持ASCII码),高度8px
//UInt8 fns_FontSmall_getAutoWidth(int ch){
//    const UInt8 * pFontLib;
//    pFontLib=fns_FontSmall_getBitmap(ch);
//    return FONT_WIDTH(pFontLib,FONT_SMALL_MINWIDTH,FONT_SMALL_MAXWIDTH);
//}

UInt8 fns_FontNormal_getAutoWidth(int wch){
    const UInt8 * pFontLib;
    pFontLib=fns_FontNormal_getBitmap(wch);
    return FONT_WIDTH(pFontLib,FONT_NORMAL_MINWIDTH,FONT_NORMAL_MAXWIDTH);
}

const FONTHANDLER FONT_SMALL_FIXED={ //只能显示字母/数字
    fns_FontSmall_getBitmap,
    fns_FontSmall_getFixedWidth,
    fns_FontSmall_draw,
};
const FONTHANDLER FONT_NORMAL_AUTO={  //可以显示任何文字
    fns_FontNormal_getBitmap,
    fns_FontNormal_getAutoWidth,
    fns_FontNormal_draw,
};
const FONTHANDLER FONT_NORMAL_FIXED={ //只能显示字母/数字
    fns_FontNormal_getBitmap,
    fns_FontNormal_getFixedWidth,
    fns_FontNormal_draw,
};

UInt8 fns_FontSmall_draw(CDC* pCDC,int xpoint,int ypoint,int ch){
	UInt8 x,y;
	UInt8 bVal;
    UInt8 const * pFontLib;
    UInt8 nWidth;
	UInt8 (*pLcdBuff)[LCD_X_MAX+1];
	UInt8 color_show,color_hide;
	if(pCDC->color==COLOR_WHITE){
		color_show=COLOR_WHITE;
		color_hide=COLOR_BLACK;
	}else if(pCDC->color==COLOR_BLACK){
		color_show=COLOR_BLACK;
		color_hide=COLOR_WHITE;
	}else if(pCDC->color==COLOR_XOR){
		color_show=COLOR_XOR;
		color_hide=COLOR_NONE;
	}else{
		color_show=COLOR_NONE;
		color_hide=COLOR_NONE;
    }
	

    pLcdBuff=pCDC->lcdBuff;
    pFontLib=pCDC->font->getBitmap(ch);
    nWidth=pCDC->font->getWidth(ch);

	for (x=0;x<nWidth;x++){
		bVal=pFontLib[x];
		for(y=0;y<FONT_SMALL_HEIGHT;y++){
			if(bVal & (0x01<<y) ){
				def_CDC_setPoint(pLcdBuff,xpoint+x,ypoint+y,color_show);
			}else{
				def_CDC_setPoint(pLcdBuff,xpoint+x,ypoint+y,color_hide);
			}
		}
	}
    return nWidth;
}

UInt8 fns_FontNormal_draw(CDC* pCDC,int xpoint,int ypoint,int ch){
	UInt8 x,y;
	UInt8 bVal;
    UInt8 const * pFontLib;
    UInt8 nWidth;
    UInt8 (*pLcdBuff)[LCD_X_MAX+1];
	UInt8 color_show,color_hide;
	if(pCDC->color==COLOR_WHITE){
		color_show=COLOR_WHITE;
		color_hide=COLOR_BLACK;
	}else if(pCDC->color==COLOR_BLACK){
		color_show=COLOR_BLACK;
		color_hide=COLOR_WHITE;
	}else if(pCDC->color==COLOR_XOR){
		color_show=COLOR_XOR;
		color_hide=COLOR_NONE;
	}else{
		color_show=COLOR_NONE;
		color_hide=COLOR_NONE;
    }
    pLcdBuff=pCDC->lcdBuff;
    pFontLib=pCDC->font->getBitmap(ch);
    nWidth=pCDC->font->getWidth(ch);
    
	for (x=0;x<nWidth;x++){
		bVal=pFontLib[x];
		for(y=0;y<8;y++){
			if(bVal & (0x01<<y) ){
				def_CDC_setPoint(pLcdBuff,xpoint+x,ypoint+y,color_show);
			}else{
				def_CDC_setPoint(pLcdBuff,xpoint+x,ypoint+y,color_hide);
			}
		}
		
		bVal= pFontLib[x+12];
		for(y=0;y<4;y++){
			if(bVal & (0x01<<y) ){
				def_CDC_setPoint(pLcdBuff,xpoint+x,ypoint+8+y,color_show);
			}else{
				def_CDC_setPoint(pLcdBuff,xpoint+x,ypoint+8+y,color_hide);
			}
		}
		#if (FONT_NORMAL_LINEMARGIN>0)
			if( (ypoint+FONT_NORMAL_HEIGHT)<LCD_Y_MAX ){
				def_CDC_setPoint(pLcdBuff,xpoint+x,ypoint+FONT_NORMAL_HEIGHT,color_hide);
			}
		#endif
	}
    return nWidth;
}



COLOR def_CDC_getPoint(UInt8 (*pLcdBuff)[LCD_X_MAX+1], LCDSIZETYPE xpiont,LCDSIZETYPE ypoint){
	UInt8 row,col,offset;
	row=ypoint/8;
	col=xpiont;
	offset=ypoint%8;
	if( pLcdBuff[row][col] & (0x01<<offset) ){
		return COLOR_BLACK;
	}else
		return COLOR_WHITE;
}
void def_CDC_setPoint(UInt8 (*pLcdBuff)[LCD_X_MAX+1],LCDSIZETYPE xpoint,LCDSIZETYPE ypoint,COLOR color){
	UInt8 row,col,offset;
    
    if( (xpoint>LCD_X_MAX) || (ypoint>LCD_Y_MAX) ) return;
    ypoint=LCD_Y_MAX-ypoint;
	row=ypoint/8;
	col=xpoint;
	offset=ypoint%8;
	if( color==COLOR_WHITE ){
		pLcdBuff[row][col] &= ~(0x01<<offset);	
	}else if( color==COLOR_BLACK ){
		pLcdBuff[row][col] |= (0x01<<offset);
	}else if( color==COLOR_XOR ){
		pLcdBuff[row][col] ^= (0x01<<offset);
	}else{
		//忽略
	}
}

int WSTRING_WIDTH(const FONTHANDLER* font,wstring strText){
    int i=0,nWidth=0;
    while(strText[i]){
        nWidth+=font->getWidth(strText[i]);
        i++;
    }
    return nWidth;
}
int STRING_WIDTH(const FONTHANDLER* font,string strText){
    int i=0,nWidth=0;
    while(strText[i]){
        nWidth+=font->getWidth(strText[i]);
        i++;
    }
    return nWidth;
}




void fns_CDC_setColor(CDC* pCDC,COLOR color){
    pCDC->color=color;
}
void fns_CDC_setFont(CDC* pCDC,FONTHANDLER* font){
    pCDC->font=font;
}

void fns_CDC_drawPoint(CDC* pCDC,UInt32 xpiont,UInt32 ypoint){
	if( (xpiont>LCD_X_MAX)||(ypoint>LCD_Y_MAX) )
		return;
    def_CDC_setPoint(pCDC->lcdBuff,xpiont,ypoint,pCDC->color);
}

void fns_CDC_drawLine(CDC* pCDC,UInt32 x1,UInt32 y1,UInt32 x2,UInt32 y2){
	int dx,dy,e;

    x1=(LCDSIZETYPE)min(x1,LCD_X_MAX);
	x2=(LCDSIZETYPE)min(x2,LCD_X_MAX);
	y1=(LCDSIZETYPE)min(y1,LCD_Y_MAX);
	y2=(LCDSIZETYPE)min(y2,LCD_Y_MAX);


	dx=x2-x1; 
	dy=y2-y1;
    
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					def_CDC_setPoint(pCDC->lcdBuff,x1,y1,pCDC->color);
					if(e>0){y1+=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					def_CDC_setPoint(pCDC->lcdBuff,x1,y1,pCDC->color);
					if(e>0){x1+=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					def_CDC_setPoint(pCDC->lcdBuff,x1,y1,pCDC->color);
					if(e>0){y1-=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					def_CDC_setPoint(pCDC->lcdBuff,x1,y1,pCDC->color);
					if(e>0){x1+=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					def_CDC_setPoint(pCDC->lcdBuff,x1,y1,pCDC->color);
					if(e>0){y1+=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					def_CDC_setPoint(pCDC->lcdBuff,x1,y1,pCDC->color);
					if(e>0){x1-=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					def_CDC_setPoint(pCDC->lcdBuff,x1,y1,pCDC->color);
					if(e>0){y1-=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					def_CDC_setPoint(pCDC->lcdBuff,x1,y1,pCDC->color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				} 
			}
		}	
	}
}
/*
void fns_CDC_drawLine(CDC* pCDC,UInt32 x1,UInt32 y1,UInt32 x2,UInt32 y2){
	LCDSIZETYPE x,y;

    x1=(LCDSIZETYPE)min(x1,LCD_X_MAX);
	x2=(LCDSIZETYPE)min(x2,LCD_X_MAX);
	y1=(LCDSIZETYPE)min(y1,LCD_Y_MAX);
	y2=(LCDSIZETYPE)min(y2,LCD_Y_MAX);
    
	if( (x2-x1)*(x2-x1) > (y2-y1)*(y2-y1) ){
		if(x2>x1){
			for( x=x1;x<=x2;x++){
				y=(y2-y1)*(x-x1)/(x2-x1)+y1;
				def_CDC_setPoint(pCDC->lcdBuff,x,y,pCDC->color);
			}
		}else{
			for( x=x2;x<=x1;x++){
				y=(y2-y1)*(x-x1)/(x2-x1)+y1;
				def_CDC_setPoint(pCDC->lcdBuff,x,y,pCDC->color);
			}
		}
	}else{
		if(y2>y1){
			for( y=y1;y<=y2;y++){
				x=(x2-x1)*(y-y1)/(y2-y1)+x1;
				def_CDC_setPoint(pCDC->lcdBuff,x,y,pCDC->color);
			}
		}else{
			for( y=y2;y<=y1;y++){
				x=(x2-x1)*(y-y1)/(y2-y1)+x1;
				def_CDC_setPoint(pCDC->lcdBuff,x,y,pCDC->color);
			}
		}		
	}
}
*/
void fns_CDC_drawRect(CDC* pCDC,UInt32 x1,UInt32 y1,UInt32 x2,UInt32 y2){
    x1=min(x1,LCD_X_MAX);
	x2=min(x2,LCD_X_MAX);
	y1=min(y1,LCD_Y_MAX);
	y2=min(y2,LCD_Y_MAX);
    
	fns_CDC_drawLine(pCDC,x1,y1,x2,y1);
	fns_CDC_drawLine(pCDC,x2,y1,x2,y2);
	fns_CDC_drawLine(pCDC,x2,y2,x1,y2);
	fns_CDC_drawLine(pCDC,x1,y2,x1,y1);
}
void fns_CDC_fillRect(CDC* pCDC,UInt32 x1,UInt32 y1,UInt32 x2,UInt32 y2){
	LCDSIZETYPE x,y;
    
    x1=(LCDSIZETYPE)min(x1,LCD_X_MAX);
	x2=(LCDSIZETYPE)min(x2,LCD_X_MAX);
	y1=(LCDSIZETYPE)min(y1,LCD_Y_MAX);
	y2=(LCDSIZETYPE)min(y2,LCD_Y_MAX);
	for(x=x1;x<x2;x++){
		for(y=y1;y<y2;y++){
            def_CDC_setPoint(pCDC->lcdBuff,x,y,pCDC->color);
		}
	}
}

UInt32 fns_CDC_drawChar(CDC* pCDC,UInt32 xpoint,UInt32 ypoint,char ch){
    return pCDC->font->draw(pCDC,xpoint,ypoint,ch);
}

UInt32 fns_CDC_drawWChar(CDC* pCDC,UInt32 xpoint,UInt32 ypoint,wchar_t ch){
    return pCDC->font->draw(pCDC,xpoint,ypoint,ch);
}

UInt32 fns_CDC_drawString(CDC* pCDC,UInt32 xpoint,UInt32 ypoint,string strText){
    UInt16 i=0;
    UInt32 s=xpoint;
    while(strText[i]){
        xpoint+=fns_CDC_drawChar(pCDC,xpoint,ypoint,strText[i]);
        i++;
    }
    return xpoint-s;
}
UInt32 fns_CDC_drawWString(CDC* pCDC,UInt32 xpoint,UInt32 ypoint,wstring strText){
    UInt16 i=0;
    UInt32 s=xpoint;
    while(strText[i]){
        xpoint+=fns_CDC_drawWChar(pCDC,xpoint,ypoint,strText[i]);
        i++;
    }
    return xpoint-s;
}

void fns_CDC_clear(CDC* pCDC,UInt8 bMask){
	memset(pCDC->lcdBuff,bMask,((LCD_Y_MAX+1)/8)*(LCD_X_MAX+1));
}

void fns_CDC_drawByte(CDC* pCDC,LCDSIZETYPE xpoint,LCDSIZETYPE ypoint,UInt8 bytVal){
	UInt8 y;
	UInt8 color_show,color_hide;
	if(pCDC->color==COLOR_WHITE){
		color_show=COLOR_WHITE;
		color_hide=COLOR_BLACK;
	}else if(pCDC->color==COLOR_BLACK){
		color_show=COLOR_BLACK;
		color_hide=COLOR_WHITE;
	}else if(pCDC->color==COLOR_XOR){
		color_show=COLOR_XOR;
		color_hide=COLOR_NONE;
	}else{
		color_show=COLOR_NONE;
		color_hide=COLOR_NONE;
    }
	
	for(y=0;y<8;y++){
    	if(bytVal & (0x01<<y) ){
			def_CDC_setPoint(pCDC->lcdBuff,xpoint,ypoint+y,color_show);
		}else{
			def_CDC_setPoint(pCDC->lcdBuff,xpoint,ypoint+y,color_hide);
		}
	}
}
void fns_CDC_drawIcon16(CDC* pCDC,LCDSIZETYPE xpoint,LCDSIZETYPE ypoint,UInt8 iconIndex){
    extern const UInt8 FONT_ICON16[][16];

	UInt8 i;
	for (i=0;i<16;i++){
		fns_CDC_drawByte(pCDC,xpoint+i,ypoint,FONT_ICON16[iconIndex][i]);
	}
}
void fns_CDC_drawIcon24(CDC* pCDC,LCDSIZETYPE xpoint,LCDSIZETYPE ypoint,UInt8 iconIndex){
    extern const UInt8 FONT_ICON24[][24];

	UInt8 i;
	for (i=0;i<24;i++){
		fns_CDC_drawByte(pCDC,xpoint+i,ypoint,FONT_ICON24[iconIndex][i]);
	}
}


////----------------------------------------------
////void lcd_draw_sign(BYTE xpoint,BYTE ypoint,BYTE coding){
////功能：画信号强度，GPRS共分5个等级(0~4)，通过coding区别；
////					GPS 也分5个等级(0~4)，紧跟其后 
////入参：位置，信号类型。 
//void fns_CDC_drawSign(CDC* pCDC,UInt8 xpoint,UInt8 ypoint,UInt8 codepage){
//    extern const UInt8 FONT_ICON24[][24];
//	UInt8  x,y;
//	UInt8  bVal;

//	for (x=0;x<24;x++){
//        if( (xpoint+x)>LCD_X_MAX ) break;
//        bVal =  FONT_ICON24[codepage][x];
//		for(y=0;y<8;y++){
//            if( (ypoint+y)>LCD_Y_MAX ) break;
//			if(bVal & (0x01<<y) ){
//                def_CDC_setPoint(pCDC->lcdBuff,xpoint+x,ypoint+y,pCDC->color);
//			}else{
//                def_CDC_setPoint(pCDC->lcdBuff,xpoint+x,ypoint+y,COLOR_NONE);
//			}
//		}
////	
////		bVal= ICONLIB_SIGN[codepage+x+24];
////		for(y=0;y<4;y++){
////            if( (ypoint+8+y)>LCD_Y_MAX ) break;
////			if(bVal & (0x01<<y) ){
////                def_CDC_setPoint(pCDC->lcdBuff,xpoint+x,ypoint+8+y,pCDC->color);
////			}else{
////				def_CDC_setPoint(pCDC->lcdBuff,xpoint+x,ypoint+8+y,COLOR_NONE);
////			}
////		}
//	}
//}

/////////////////////其它常用函数////////////////////
extern const char HEXSTR[];


UInt32 fns_CDC_drawDate(CDC* pCDC,UInt32 x,UInt32 y,DATETIME* pTime){
    UInt32 s=x;
	x+=fns_CDC_drawChar(pCDC,x,y,'2');
	x+=fns_CDC_drawChar(pCDC,x,y,'0');
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[pTime->Year/10]);
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[pTime->Year%10]);
	x+=fns_CDC_drawChar(pCDC,x,y,'-');
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[pTime->Month/10]);
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[pTime->Month%10]);
	x+=fns_CDC_drawChar(pCDC,x,y,'-');
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[pTime->Day/10]);
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[pTime->Day%10]);
    return x-s;
}
UInt32 fns_CDC_drawTime(CDC* pCDC,UInt32 x,UInt32 y,DATETIME* pTime){
    UInt32 s=x;   
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[pTime->Hour/10]);
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[pTime->Hour%10]);
	x+=fns_CDC_drawChar(pCDC,x,y,':');
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[pTime->Minute/10]);
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[pTime->Minute%10]);
	x+=fns_CDC_drawChar(pCDC,x,y,':');
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[pTime->Second/10]);
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[pTime->Second%10]);
    return x-s;
}
UInt32 fns_CDC_drawDateTime(CDC* pCDC,UInt32 x,UInt32 y,DATETIME* pTime){
    UInt32 s=x;
	x+=fns_CDC_drawDate(pCDC,x,y,pTime);
	x+=fns_CDC_drawChar(pCDC,x,y,' ');
 	x+=fns_CDC_drawTime(pCDC,x,y,pTime);
    return x-s;
}


UInt32 fns_CDC_drawHexs(CDC* pCDC,UInt32 x,UInt32 y,UInt8* pHex,UInt8 nCount){
	UInt8 i;
    UInt32 s=x;
	for(i=0;i<nCount;i++){
		x+=fns_CDC_drawHex(pCDC,x,y,pHex[i]);
	}
    return x-s;
}
UInt32 fns_CDC_drawHex(CDC* pCDC,UInt32 x,UInt32 y,UInt8 nHexVal){
    UInt32 s=x;
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[nHexVal>>4]);
	x+=fns_CDC_drawChar(pCDC,x,y,HEXSTR[nHexVal&0x0F]);
    return x-s;
}


