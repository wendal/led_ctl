#ifndef __system__
#define __system__

typedef   unsigned short  WORD;
typedef   unsigned char   BYTE;
typedef   unsigned long   DWORD;

#define   NULL ((void *)0)

#define   ROOT_PLAY          0x11      /*仅用于播放*/
#define   ROOT_DOWNLOAD      0x12      /*保存并播放，由于主板上有Flash芯片，因此可将显示内容保存在主板上，当断电重新启动时，显示屏可显示上次发送的数据（由于Flash有一定的写入次数限制，因此如果没有必要请不要用ROOT_DOWNLOAD）*/
#define   ROOT_FONTSET       0x14
 
#define   OBJECT_LEAF      0x30
#define   OBJECT_WINDOW    0x31
#define   OBJECT_DATETIME  0x32
#define   OBJECT_STRING    0x33
#define   OBJECT_EFFECT    0x34
#define   OBJECT_FONTSET   0xEE
#define   OBJECT_PIXELS    0x36
#define   OBJECT_PIXELSET  0x37
#define   OBJECT_MOVIE     0x38
#define   OBJECT_COUNTDOWN 0x39
#define   OBJECT_COUNTUP   0x3A
#define   OBJECT_PICTURE   0x3B
#define   OBJECT_CLOCK     0x60
#define   OBJECT_CAMPAIGN  0x61
#define   OBJECT_TEMPERATURE 0x50
#define   OBJECT_HUMIDITY    0x51

#define   SCREEN_UNICOLOR    0x01
#define   SCREEN_COLOR       0x02
#define   SCREEN_GRAY        0x03

//点阵类型定义，针对双色屏，都应该设置为PIXELS_COLOR
#define PIXELS_RED     1   //单红色点阵(单色屏时使用)
#define PIXELS_GREEN   2   //单绿色点阵
#define PIXELS_YELLOW  3   //单黄色点整
#define PIXELS_COLOR   4   //双色点阵(红,绿)

typedef struct _RGB{
  BYTE   r;
  BYTE   g;
  BYTE   b;
}TRGB;
typedef TRGB * PRGB;

typedef struct FONTSET{
  WORD  id;             //对象标识
  WORD  width;          //点阵宽度
  DWORD size;           //对象字节数
  BYTE  name[16];       //字符集名称
}TFontSet;
typedef TFontSet * PFontSet;

typedef struct ROOT{
  WORD  id;              //数据流类型
  WORD  count;           //页面数目
  DWORD size;            //整个数据流字节数(包含数据字节数和该元素本身字节数之和)
  WORD  type;            //要求的显示屏类型（单色、双色、全彩）？具体
  WORD  Reserved;
}TRoot, *PRoot;

typedef struct LEAF{
  WORD  id;              //页面标识
  WORD  count;           //对象数目
  DWORD time;            //页面显示时间单位为毫秒
  DWORD Reserved;
  DWORD size;            //对象字节数
}TLeaf, *PLeaf;

typedef struct RECT{      /*以左上角为原点*/
  short  left;
  short  top;
  short  right;
  short  bottom;
}TRect, *PRect;

typedef struct PIXELS{
  WORD   id;            //对象标识
  WORD   width;         //点阵宽度
  WORD   height;        //点阵高度
  WORD   bits;          //每象素位数
  DWORD  size;          //对象大小
}TPixels, *PPixels;

typedef struct PIXELSET{
  WORD   id;            //对象标识
  WORD   count;         //TPixels对象数目
  DWORD  size;          //对象字节数
}TPixelSet, *PPixelSet;

typedef struct PLAYPARAM{
  short  x,y;
  DWORD  time;
}TPlayParam;

typedef struct OBJECT{
  WORD    id;             //对象标识
  WORD    transparent;    //是否透明：0=不透明 1=透明
  DWORD   size;           //对象字节数
  TRect   rect;           //对象显示区域
}TObject, *PObject;
                          //显示区域设置
typedef struct STRING{
  TObject   object;       //对象的一般定义
  WORD      method;       //显示方式
  WORD      speed;        //显示速度
  DWORD     Reserved[2];
  DWORD     color;        //字符颜色：0xff=红色  0xFF00=绿色  0xFF0000=蓝色
  DWORD     fontset;      //字符集(一般情况 0=16点阵 1=24点阵)
}TString, *PString;

typedef struct WINDOW{
  TObject     object;   //对象的一般定义
  WORD        method;   //显示方式
  WORD        speed;    //显示速度
  TPlayParam  param;    //预留参数空间给单片机
}TWindow, *PWindow;

#endif