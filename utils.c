#include "stdio.h"
#include "system.h"

//获得点阵数据
//  obuffer是转换后的点阵数据
//  ibuffer是源图片数据，数据格式为每个像素点包含(RGB)3个字节
//  cx,cy是源图片的大小
DWORD MakePixels(BYTE* obuffer, BYTE* ibuffer, long cx, long cy)
{
  long x, y;
  PRGB rgb;
  DWORD shift;
  PPixels pixels=(PPixels)obuffer;
  DWORD* ptr=(DWORD*)(obuffer+sizeof(TPixels));
  
  pixels->id=OBJECT_PIXELS;
  pixels->bits=PIXELS_COLOR;
  pixels->width=(WORD)cx;
  pixels->height=((cy+0xf)&(~0xf));
  pixels->size=sizeof(TPixels)+pixels->width*(pixels->height>>2);
  rgb=(PRGB)ibuffer;
  *ptr=0;
  for (x=0; x<cx; x++)
  {
    shift=0x80000000;
    for (y=0; y<cy; y++)
    {
      if (rgb->g>0) (*ptr)|=shift;
      shift>>=1;
      if (rgb->r>0) (*ptr)|=shift;
      shift>>=1;
      if (shift==0)
      {
        shift=0x80000000;
        ptr++;
        *ptr=0;
      }
      rgb=(PRGB)((DWORD)rgb+3);
    }
    if (shift!=0x80000000)
	{
	  ptr++;
	  *ptr=0;
	}
  }
  return pixels->size;
}

void BitToPixels(DWORD *dst, long x, long y, long width, long height, BYTE *src, long xSrc, long ySrc, long srcWidth, long srcHeight, DWORD MatrixWidth, DWORD MatrixHeight)
{
  int   cx,cy;
  BYTE  shift;
  DWORD Mask;
  DWORD *dstTmp;
  BYTE  *srcTmp;

  for (cx=0; cx<width; cx++)
  {
    shift=0x80>>(ySrc & 7);
    Mask=0xC0000000>>((y & 0xF)<<1);
    srcTmp=src+(xSrc+cx)*(srcHeight>>3)+(ySrc>>3);
    dstTmp=dst+(x+cx)*(MatrixHeight>>4)+(y>>4);
    for (cy=0; cy<height; cy++)
    {
      if (*srcTmp & shift) *dstTmp|=Mask;
      else *dstTmp&=~Mask;
      shift>>=1;
      if (shift==0)
      {
        shift=0x80;
        srcTmp++;
      }
      Mask>>=2;
      if (Mask==0)
      {
        Mask=0xC0000000;
        dstTmp++;
      }
    }
  }
}

//获得字符串的显示点阵大小
//  str是字符串
//  ox是计算得到的点阵宽度
//  oy是计算得到的点阵高度
//  cx是一个汉字对应的宽度
void GetTextRect(BYTE *str, WORD* ox, WORD* oy, WORD cx)
{
  int i=0;
  WORD maxwidth=0, width=0;
  *oy=cx;
  while (str[i]!=0)
  {
    if (str[i]>=0xA0)
    {
      if (str[i+1]>=0xA0)
      {
        i+=2;
        width+=cx;
      }
      else
      {
        switch (str[i]){
    	  case 0x0d: //回车
            if (width>maxwidth) maxwidth=width;
    	    width=0;
            break;
          case 0x0a: //换行
            (*oy)+=cx;
            break;
          default:
            width+=cx>>1;
        }
        i++;
      }
    }
    else
    {
      switch (str[i]){
   	    case 0x0d: //回车
          if (width>maxwidth) maxwidth=width;
    	  width=0;
    	  break;
        case 0x0a: //换行
    	  (*oy)+=cx;
    	  break;
    	default:
          width+=cx>>1;
      }
      i++;
    }
  }
  if (width>maxwidth) maxwidth=width;
  *ox=maxwidth;
}

DWORD MakeFontSetText(BYTE *obuffer, char* str, char* fontfile, DWORD fontcolor)
{
  PPixels  pixels=(PPixels)obuffer;
  long     x,y,ASCII_SIZE,ASCII_BYTES,ASCII_WIDTH,CHINA_BYTES;
  TFontSet fontset;
  FILE*    fp;
  DWORD    size;
  BYTE     chbuffer[64*1024];
  //char     spath[256];
  
  fp=fopen(fontfile, "r");
  size=fread((BYTE*)&fontset, 1, sizeof(TFontSet), fp);

  pixels->id=OBJECT_PIXELS;
  pixels->bits=PIXELS_COLOR;
  GetTextRect(str, &pixels->width, &pixels->height, fontset.width);
  if (pixels->height & 0xF) pixels->height=(pixels->height & ~0xF)+0x10;
  pixels->size=((pixels->width*pixels->height)>>2)+sizeof(TPixels);
  if ((fontcolor & 0xFF00) && (fontcolor & 0xFF)) pixels->bits=PIXELS_YELLOW;
  else if (fontcolor & 0xFF00) pixels->bits=PIXELS_GREEN;
  else pixels->bits=PIXELS_RED;
  if ((fontcolor & 0xFF)==0 && (fontcolor & 0xFF00)==0)
  {
    memset((BYTE*)((DWORD)pixels+sizeof(TPixels)), 0, pixels->size-sizeof(TPixels));
	fclose(fp);
    return pixels->size;
  }

  memset((BYTE*)((DWORD)pixels+sizeof(TPixels)), 0, pixels->size-sizeof(TPixels));

  CHINA_BYTES=(fontset.width>>3)*fontset.width;
  ASCII_WIDTH=fontset.width>>1;
  ASCII_BYTES=ASCII_WIDTH*(fontset.width>>3);
  ASCII_SIZE=ASCII_BYTES<<8;
  
  x=0;
  y=0;
  while (*str)
  {
    if (*str>=0xA0)
    {
      if (*(str+1)>=0xA0)
      {
		fseek(fp, sizeof(TFontSet)+ASCII_SIZE+(*str-0xA1)*94*CHINA_BYTES+(*(str+1)-0xA1)*CHINA_BYTES, SEEK_SET);
		size=fread(chbuffer, 1, CHINA_BYTES, fp);
        BitToPixels((DWORD*)((DWORD)pixels+sizeof(TPixels)), x, y, fontset.width, fontset.width, chbuffer, 0, 0, fontset.width, fontset.width, pixels->width, pixels->height);
        x+=fontset.width;
        str+=2;
      }
      else
      {
		fseek(fp, sizeof(TFontSet)+*str*ASCII_BYTES, SEEK_SET);
		size=fread(chbuffer, 1, ASCII_BYTES, fp);
        BitToPixels((DWORD*)((DWORD)pixels+sizeof(TPixels)), x, y, ASCII_WIDTH, fontset.width, chbuffer, 0, 0, ASCII_WIDTH, fontset.width, pixels->width, pixels->height);
        x+=ASCII_WIDTH;
        str++;
      }
    }
    else
    {
      switch (*str){
        case 0x0d: //回车
          x=0;
          break;
        case 0x0a: //换行
          y+=fontset.width;
          break;
        default:
          fseek(fp, sizeof(TFontSet)+*str*ASCII_BYTES, SEEK_SET);
          size=fread(chbuffer, 1, ASCII_BYTES, fp);
          BitToPixels((DWORD*)((DWORD)pixels+sizeof(TPixels)), x, y, ASCII_WIDTH, fontset.width, chbuffer, 0, 0, ASCII_WIDTH, fontset.width, pixels->width, pixels->height);
          x+=ASCII_WIDTH;
          break;
      }
      str++;
    }
  }
  fclose(fp);
  return pixels->size;
}

