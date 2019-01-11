#ifndef _FONT_H
#define _FONT_H

//图标
#define FONT_PIC_WIDTH_16  16
#define FONT_PIC_WIDTH_24  24
//小字体
#define FONT_SMALL_MINWIDTH       2
#define FONT_SMALL_MAXWIDTH       6
#define FONT_SMALL_HEIGHT         8

#define FONT_SMALL_DIGITWIDTH     5

//正常字体(参数必须与字库匹配)
#define FONT_NORMAL_MINWIDTH       3
#define FONT_NORMAL_MAXWIDTH       12
#define FONT_NORMAL_HEIGHT         12
#define FONT_NORMAL_LINEMARGIN     0

#define FONT_NORMAL_DIGITWIDTH     6

#define FONT_POWER_0            0
#define FONT_POWER_1            1
#define FONT_POWER_2            2
#define FONT_POWER_3            3
#define FONT_BLUETOOTH          4
#define FONT_BLUETOOTH_DISABLE  5

#define FONT_GSM_0            0
#define FONT_GSM_1            1
#define FONT_GSM_2            2
#define FONT_GSM_3            3
#define FONT_GSM_4            4

#define FONT_STARTADDR_25           0
#define FONT_STARTADDR_4E          (9*512l)
#define FONT_UCHARSIZE              24
#define FONT_STRINGADDR_LIST       (896*512)
#define FONT_STRINGADDR_CHARS      (900*512)


UInt8 FONT_WIDTH(UInt8 const * pFontLib,UInt8 nMinWidth,UInt8 nMaxWidth);

const UInt8 * fns_FontSmall_getBitmap(int ch);
const UInt8 * fns_FontNormal_getBitmap(int ch);
UInt8 fns_FontNormal_getWidth(int ch);
UInt8 fns_FontSmall_getWidth(int ch);


#endif
