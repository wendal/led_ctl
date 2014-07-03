#ifndef __system__
#define __system__

typedef   unsigned short  WORD;
typedef   unsigned char   BYTE;
typedef   unsigned long   DWORD;

#define   NULL ((void *)0)

#define   ROOT_PLAY          0x11      /*�����ڲ���*/
#define   ROOT_DOWNLOAD      0x12      /*���沢���ţ�������������FlashоƬ����˿ɽ���ʾ���ݱ����������ϣ����ϵ���������ʱ����ʾ������ʾ�ϴη��͵����ݣ�����Flash��һ����д��������ƣ�������û�б�Ҫ�벻Ҫ��ROOT_DOWNLOAD��*/
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

//�������Ͷ��壬���˫ɫ������Ӧ������ΪPIXELS_COLOR
#define PIXELS_RED     1   //����ɫ����(��ɫ��ʱʹ��)
#define PIXELS_GREEN   2   //����ɫ����
#define PIXELS_YELLOW  3   //����ɫ����
#define PIXELS_COLOR   4   //˫ɫ����(��,��)

typedef struct _RGB{
  BYTE   r;
  BYTE   g;
  BYTE   b;
}TRGB;
typedef TRGB * PRGB;

typedef struct FONTSET{
  WORD  id;             //�����ʶ
  WORD  width;          //������
  DWORD size;           //�����ֽ���
  BYTE  name[16];       //�ַ�������
}TFontSet;
typedef TFontSet * PFontSet;

typedef struct ROOT{
  WORD  id;              //����������
  WORD  count;           //ҳ����Ŀ
  DWORD size;            //�����������ֽ���(���������ֽ����͸�Ԫ�ر����ֽ���֮��)
  WORD  type;            //Ҫ�����ʾ�����ͣ���ɫ��˫ɫ��ȫ�ʣ�������
  WORD  Reserved;
}TRoot, *PRoot;

typedef struct LEAF{
  WORD  id;              //ҳ���ʶ
  WORD  count;           //������Ŀ
  DWORD time;            //ҳ����ʾʱ�䵥λΪ����
  DWORD Reserved;
  DWORD size;            //�����ֽ���
}TLeaf, *PLeaf;

typedef struct RECT{      /*�����Ͻ�Ϊԭ��*/
  short  left;
  short  top;
  short  right;
  short  bottom;
}TRect, *PRect;

typedef struct PIXELS{
  WORD   id;            //�����ʶ
  WORD   width;         //������
  WORD   height;        //����߶�
  WORD   bits;          //ÿ����λ��
  DWORD  size;          //�����С
}TPixels, *PPixels;

typedef struct PIXELSET{
  WORD   id;            //�����ʶ
  WORD   count;         //TPixels������Ŀ
  DWORD  size;          //�����ֽ���
}TPixelSet, *PPixelSet;

typedef struct PLAYPARAM{
  short  x,y;
  DWORD  time;
}TPlayParam;

typedef struct OBJECT{
  WORD    id;             //�����ʶ
  WORD    transparent;    //�Ƿ�͸����0=��͸�� 1=͸��
  DWORD   size;           //�����ֽ���
  TRect   rect;           //������ʾ����
}TObject, *PObject;
                          //��ʾ��������
typedef struct STRING{
  TObject   object;       //�����һ�㶨��
  WORD      method;       //��ʾ��ʽ
  WORD      speed;        //��ʾ�ٶ�
  DWORD     Reserved[2];
  DWORD     color;        //�ַ���ɫ��0xff=��ɫ  0xFF00=��ɫ  0xFF0000=��ɫ
  DWORD     fontset;      //�ַ���(һ����� 0=16���� 1=24����)
}TString, *PString;

typedef struct WINDOW{
  TObject     object;   //�����һ�㶨��
  WORD        method;   //��ʾ��ʽ
  WORD        speed;    //��ʾ�ٶ�
  TPlayParam  param;    //Ԥ�������ռ����Ƭ��
}TWindow, *PWindow;

#endif