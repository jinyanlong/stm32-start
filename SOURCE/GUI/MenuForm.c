#include <includes.h>
#include "Font.h"
#include "FormCDC.h"
#include "MenuForm.h"


void drv_MenuForm_adjustIndex(MENUFORM* pThis,int linecount){
	//调整首行
	if( (pThis->curIndex>=pThis->firstIndex) && (pThis->curIndex<pThis->firstIndex+linecount) ){
		//保持首行不变
	}else{
		if(pThis->curIndex<pThis->firstIndex){
			pThis->firstIndex=pThis->curIndex;
		}else{
			pThis->firstIndex=pThis->curIndex-(linecount-1);
		}
	}
}



void drv_MenuForm_showArrow(MENUFORM* pThis,CDC* pCDC){
    extern const UInt8 FONT_ARROW[][24];

//	if(pThis->firstIndex>0){
//        fns_CDC_drawUTFFont(&_formCDC,LCD_X_MAX-FONT_WCHAR_WIDTH,FONT_CHAR_HEIGHT,FONT_ARROW[S_CHAR_ARROWUP]);
//    }
//	if( pThis->itemCount > (4+pThis->firstIndex) ){
//		fns_CDC_drawUTFFont(&_formCDC,LCD_X_MAX-FONT_WCHAR_WIDTH,4*FONT_CHAR_HEIGHT,FONT_ARROW[S_CHAR_ARROWDOWN]);
//	}
//	drv_Form_xorLine(pThis->curIndex-pThis->firstIndex+1);

    //当前行提示,不采用反显,太耗电,所以用*代替
   	drv_FormCDC_drawChar(pCDC,(pThis->curIndex-pThis->firstIndex)+1,0,'*');
}

void drv_MenuForm_onOK(MENUFORM* pThis){
    if(pThis->menuItems[pThis->curIndex].onItem){
       pThis->menuItems[pThis->curIndex].onItem(pThis);
       drv_Form_redraw();
    }
}
void drv_MenuForm_onDigit(MENUFORM* pThis,int index){
     if(pThis->itemCount > index){
        pThis->curIndex=index;
        drv_MenuForm_onOK(pThis);
     }
}
void drv_MenuForm_onChar(BASEFORM* pBase,UInt32 keyVal){
    MENUFORM* pThis=(MENUFORM*)pBase;
    switch(keyVal){
    case KEYVALUE_OK:
        drv_MenuForm_onOK(pThis);
        break;

	default:
		drv_Form_onCharDefault(pBase,keyVal);
		break;
     }
}

//默认的菜单条显示方式,菜单必须少于9条
void drv_MenuForm_showItem(CDC* pCDC,wstring pTitle,int firstIndex,int rowIndex){
    if(firstIndex+rowIndex>=9){
        drv_FormCDC_drawChar(pCDC,rowIndex+1,1*8,'1');
        drv_FormCDC_drawChar(pCDC,rowIndex+1,2*8,(firstIndex+rowIndex+1)%10+'0');
        drv_FormCDC_drawChar(pCDC,rowIndex+1,3*8,'.');
        drv_FormCDC_drawWString(pCDC,rowIndex+1,4*8,pTitle);
    }else{
        drv_FormCDC_drawChar(pCDC,rowIndex+1,1*8,(firstIndex+rowIndex)+'1');
        drv_FormCDC_drawChar(pCDC,rowIndex+1,2*8,'.');
        drv_FormCDC_drawWString(pCDC,rowIndex+1,3*8,pTitle);
    }
}
void drv_MenuForm_onShowItem(MENUFORM* pThis,CDC* pCDC,const MENUITEM* pItem,int rowIndex){
    drv_MenuForm_showItem(pCDC,LoadWString(pItem->titleId),pThis->firstIndex,rowIndex);
}
void drv_MenuForm_onShow(BASEFORM* pBase,CDC* pCDC){
	int rowIndex;
    const MENUITEM* pItem;
    MENUFORM* pThis=(MENUFORM*)pBase;

	drv_MenuForm_adjustIndex(pThis,FORM_MAXLINE);
	for(rowIndex=0;rowIndex<FORM_MAXLINE && ((pThis->firstIndex+rowIndex)<pThis->itemCount) ;rowIndex++){
        pItem=&pThis->menuItems[pThis->firstIndex+rowIndex];
        if(pItem->onShow==NULL){
            drv_MenuForm_onShowItem(pThis,pCDC,pItem,rowIndex);
        }else{
            pItem->onShow(pThis,pCDC,pItem,rowIndex);
        }
	}
	drv_MenuForm_showArrow(pThis,pCDC);
}
