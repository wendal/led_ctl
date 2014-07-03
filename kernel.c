#include "stdio.h"
#include "system.h"
#include "packet.h"
#include "utils.h"

DWORD LeafSeek=0;
DWORD BufferSeek=0;

void DoMakeRoot(PRoot root)
{
  root->id=ROOT_PLAY;
  root->count=0;
  root->size=sizeof(TRoot);
  root->type=SCREEN_COLOR; //SCREEN_COLOR;
  root->Reserved=0;
  LeafSeek=0;
  BufferSeek=sizeof(TRoot);
}

void DoAddLeaf(PRoot root, DWORD time)
{
  leaf=(PLeaf)((DWORD)root+BufferSeek);

  leaf->id=OBJECT_LEAF;
  leaf->count=0;
  leaf->size=sizeof(TLeaf);
  leaf->time=time;
  leaf->Reserved=0;

  root->count++;
  root->size+=leaf->size;
  LeafSeek=BufferSeek;
  BufferSeek+=leaf->size;
}

//�������������ʾ
//  rect		������ʾ������
//  str			��ʾ����������
//  font		��ʾ�����壬=0Ϊ16����=1Ϊ24����
//  method		������ʽ
//  color		������ʾ����ɫ
void DoAddString(PRoot root, RECT* rect, char* str, int font, int method, unsigned long color)
{
  PLeaf leaf;
  PString string;

  if (LeafSeek==0) return;
  leaf=(PLeaf)((DWORD)root+LeafSeek);
  string=(PString)((DWORD)root+BufferSeek);

  string->object.size=sizeof(TString)+strlen(str)+1;  //��һ��Ϊ��������ֹ��(0)��λ��
  if (string->object.size & 3)  //�ж��������ݳ����Ƿ�4�ֽڶ���
  {
    string->object.size&=~3;    //�����������ж���
    string->object.size+=4;
  }
  
  string->object.id=OBJECT_STRING;
  string->object.rect.left=(short)rect->left;
  string->object.rect.top=(short)rect->top;
  string->object.rect.right=(short)rect->right;
  string->object.rect.bottom=(short)rect->bottom;
  string->object.transparent=1;
  string->fontset=font;                // 0 = 16����,  1 = 24����
  string->color=color;                 //��ɫ(ȷ�����κ���ʾ���϶�����ʾ, ��: �������ϲ�����ʾ��ɫ)
  string->method=method;               //������ʽ
  string->speed=1;                     //1���ٶ�
  string->Reserved[0]=0;
  string->Reserved[1]=0;

  memcpy((void*)((DWORD)string+sizeof(TString)), str, strlen(str)+1);

  leaf->count++;
  leaf->size+=string->object.size;
  root->size+=string->object.size;
  BufferSeek+=string->object.size;
}

//���ͼƬ��ʾ
//  rect		������ʾ������
//  method      ������ʽ
//  width       ͼƬ���
//  height      ͼƬ�߶�
//  data        ͼƬ��������
void DoAddWindow(PRoot root, RECT* rect, int method, WORD width, WORD height, BYTE* data)
{
  PLeaf leaf;
  PWindow win;
  DWORD size;

  if (LeafSeek==0) return;
  leaf=(PLeaf)((DWORD)root+LeafSeek);
  win=(PWindow)((DWORD)root+BufferSeek);

  win->object.id=OBJECT_WINDOW;
  win->object.rect.left=(short)rect->left;
  win->object.rect.top=(short)rect->top;
  win->object.rect.right=(short)rect->right;
  win->object.rect.bottom=(short)rect->bottom;
  win->object.transparent=1;
  win->object.size=sizeof(TWindow);
  win->method=method;               //������ʽ
  win->speed=1;                     //1���ٶ�
  win->param.time=0;
  win->param.x=0;
  win->param.y=0;

  size=MakePixels((BYTE*)((DWORD)win+sizeof(TWindow)), data, width, height);

  win->object.size+=size;

  leaf->count++;
  leaf->size+=win->object.size;
  root->size+=win->object.size;
  BufferSeek+=win->object.size;
}

//���Windows����������ʾ
//  rect		������ʾ������
//  method      ������ʽ
//  fontname	��������
//  fontsize	�����С
//  fontcolor	������ɫ
//  str			��ʾ����������
//  ****  �˺�������ֻ����Windows�����±�������  ****
void DoAddText(PRoot root, RECT* rect, int method, char* fontname, DWORD fontsize, DWORD fontcolor, char* str)
{
	SIZE asize;
	RECT R;
	HDC hDesktopDC, hMemDC;
	HBITMAP hBitmap, hOldBitmap;
	HFONT hFont, hOldFont;
	HBRUSH hBrush, hOldBrush;

	BITMAPINFO bi;
	char* bits;
	char* buffer;
	int scancount;
	DWORD linesize;
	long x, y;
	BYTE* src;
	BYTE* dst;

	hDesktopDC=GetDC(NULL);
	hMemDC=CreateCompatibleDC(hDesktopDC);
	hBitmap=CreateCompatibleBitmap(hDesktopDC, 1, 1);
	hOldBitmap=(HBITMAP)SelectObject(hMemDC, hBitmap);
	hFont=CreateFont(-MulDiv(fontsize, GetDeviceCaps(hMemDC, LOGPIXELSY), 72), 0, 0, 0, 400, 0, 0, 0, 
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH&FF_SWISS, fontname);
	hOldFont=(HFONT)SelectObject(hMemDC, hFont);
	hBrush=CreateSolidBrush(RGB(0,0,0));
	hOldBrush=(HBRUSH)SelectObject(hMemDC, hBrush);
	GetTextExtentPoint32(hMemDC, str, strlen(str), &asize);
	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hBitmap);

	hBitmap=CreateCompatibleBitmap(hDesktopDC, asize.cx, asize.cy);
	hOldBitmap=(HBITMAP)SelectObject(hMemDC, hBitmap);
	R.left=0;
	R.top=0;
	R.right=asize.cx;
	R.bottom=asize.cy;
	FillRect(hMemDC, &R, hBrush);
	SelectObject(hMemDC, hOldBrush);
	DeleteObject(hBrush);

	SetTextColor(hMemDC, fontcolor);
	SetBkColor(hMemDC, RGB(0,0,0));
	TextOut(hMemDC, 0, 0, str, strlen(str));

	BitBlt(hDesktopDC, 0, 0, asize.cx, asize.cy, hMemDC, 0, 0, SRCCOPY);

	memset(&bi, 0, sizeof(BITMAPINFO));
	bi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth=asize.cx;
	bi.bmiHeader.biHeight=asize.cy;
	bi.bmiHeader.biPlanes=1;
	bi.bmiHeader.biBitCount=24;
	bi.bmiHeader.biCompression=BI_RGB;
	bi.bmiHeader.biSizeImage=0;
	bi.bmiHeader.biXPelsPerMeter=0;
	bi.bmiHeader.biYPelsPerMeter=0;
	bi.bmiHeader.biClrUsed=0;
	bi.bmiHeader.biClrImportant=0;

	linesize=(bi.bmiHeader.biWidth*bi.bmiHeader.biBitCount+31)/32*4;
	bits=(char*)malloc(bi.bmiHeader.biHeight*linesize);
	scancount=GetDIBits(hMemDC, hBitmap, 0, asize.cy, bits, &bi, DIB_RGB_COLORS);

	//------------------------------------------------------------------------------
	//�˶δ��룬��������������
	buffer=(char*)malloc(asize.cx*asize.cy*3);
	for (y=0; y<asize.cy; y++)
	{
		for (x=0; x<asize.cx; x++)
		{
			src=(BYTE*)((DWORD)bits+(asize.cy-y-1)*linesize+x*3+2);
			dst=(BYTE*)((DWORD)buffer+(asize.cy*3)*x+y*3);
			*dst++=*src--;
			*dst++=*src--;
			*dst=*src;
		}
	}
	//-------------------------------------------------------------------------------

	DoAddWindow(root, rect, method, (WORD)asize.cx, (WORD)asize.cy, buffer);

	free(buffer);
	free(bits);

	SelectObject(hMemDC, hOldFont);
	DeleteObject(hFont);
	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);
	ReleaseDC(NULL, hDesktopDC);
}

//����ֿ��ļ�������ʾ
//  rect		������ʾ������
//  str			��ʾ����������
//  fontfile	�ֿ��ļ�
//  method      ������ʽ
//  color		������ʾ����ɫ
void DoAddFontSetText(PRoot root, RECT* rect, char* str, char* fontfile, int method, unsigned long color)
{
  PLeaf leaf;
  PWindow win;
  DWORD size;

  if (LeafSeek==0) return;
  leaf=(PLeaf)((DWORD)root+LeafSeek);
  win=(PWindow)((DWORD)root+BufferSeek);

  win->object.id=OBJECT_WINDOW;
  win->object.rect.left=(short)rect->left;
  win->object.rect.top=(short)rect->top;
  win->object.rect.right=(short)rect->right;
  win->object.rect.bottom=(short)rect->bottom;
  win->object.transparent=1;
  win->object.size=sizeof(TWindow);
  win->method=method;               //������ʽ
  win->speed=1;                     //1���ٶ�
  win->param.time=0;
  win->param.x=0;
  win->param.y=0;

  //�������ļ�ȡ�����ֵ�����ʾ
  size=MakeFontSetText((BYTE*)((DWORD)win+sizeof(TWindow)), str, fontfile, color);

  win->object.size+=size;

  leaf->count++;
  leaf->size+=win->object.size;
  root->size+=win->object.size;
  BufferSeek+=win->object.size;
}

//������ʼ������
DWORD DoBeginPacket(BYTE* obuf, BYTE dstAddr)
{
	TPKG_Header header;
	DWORD len;
	DWORD i;

	header.Command=PKC_BEGIN;
	header.dstAddr=dstAddr;
	header.srcAddr=0;
	header.SerialNo=0;
	len=Pack((BYTE*)&header,obuf,sizeof(TPKG_Header));

    printf("Begin Packet:\n");
    for (i=0; i<len; i++) printf("%0.2X ",obuf[i]);
    printf("\n");
    printf("\n");

	return len;
}

DWORD DoDataPacket(BYTE* ibuf, BYTE* obuf, BYTE dstAddr)
{
	TPKG_Header header;
	DWORD len, I;
	BYTE* p=obuf;
	PRoot root=(PRoot)ibuf;
	DWORD i;

	header.Command=PKC_DATA;
	header.dstAddr=dstAddr;
	header.srcAddr=0;

	I=0;
	len=0;
	header.SerialNo=1;
	while (I<root->size)
	{
		if (root->size-I>PKP_LENGTH)
		{
			len+=PackEx((BYTE*)&header, sizeof(TPKG_Header), ibuf+I, p, PKP_LENGTH);
		}
		else
		{
			len+=PackEx((BYTE*)&header, sizeof(TPKG_Header), ibuf+I, p, (WORD)(root->size-I));
		}
		I+=PKP_LENGTH;
		header.SerialNo++;
		p=obuf+len;
	}

    printf("Data Packet:\n");
    for (i=0; i<len; i++) printf("%0.2X ",obuf[i]);
    printf("\n");
    printf("\n");

	return len;
}

DWORD GetDataPacketCount(BYTE* ibuf)
{
    PRoot root=(PRoot)ibuf;
    DWORD K = root->size/PKP_LENGTH;
    if ((root->size%PKP_LENGTH) > 0) return K + 1;
    else return K;
}

DWORD DoOneDataPacket(BYTE* ibuf, DWORD index, BYTE* obuf, BYTE dstAddr)
{
	TPKG_Header header;
	DWORD len, I;
	BYTE* p=obuf;
	PRoot root=(PRoot)ibuf;
	DWORD i;

	header.Command=PKC_DATA;
	header.dstAddr=dstAddr;
	header.srcAddr=0;

	I=PKP_LENGTH*(index-1);
	header.SerialNo=index;
	if (I<root->size)
	{
		if (root->size-I>PKP_LENGTH)
		{
			len=PackEx((BYTE*)&header, sizeof(TPKG_Header), ibuf+I, p, PKP_LENGTH);
		}
		else
		{
			len=PackEx((BYTE*)&header, sizeof(TPKG_Header), ibuf+I, p, (WORD)(root->size-I));
		}
	}

    printf("One Data Packet:\n");
    for (i=0; i<len; i++) printf("%0.2X ",obuf[i]);
    printf("\n");

	return len;
}

//���ɽ���������
DWORD DoEndPacket(BYTE* obuf, DWORD index, BYTE dstAddr)
{
	TPKG_Header header;
	DWORD len;
	DWORD i;

	header.Command=PKC_END;
	header.dstAddr=dstAddr;
	header.srcAddr=0;
	header.SerialNo=index;
	len=Pack((BYTE*)&header,obuf,sizeof(TPKG_Header));

    printf("End Packet:\n");
    for (i=0; i<len; i++) printf("%0.2X ",obuf[i]);
    printf("\n");
    printf("\n");

	return len;
}

int MakeString(BYTE* oBuf)
{
	BYTE* p=oBuf;
	DWORD len;
	BYTE  buffer[65535];    //����һ���ڴ���
	RECT  R;

	DoMakeRoot((PRoot)buffer);
    //���ҳ��
	DoAddLeaf((PRoot)buffer, 3600000);
    //�������
	//SetRect(&R, 0, 0, 64, 16);
	//DoAddString((PRoot)buffer, &R, "01234", 0, 1, 0xFFFFFF);
    //�����һ������
	SetRect(&R, 0, 16, 64, 32);
	DoAddString((PRoot)buffer, &R, "������", 0, 2, 0xFFFFFF);

    len=0;
	len+=DoBeginPacket(oBuf+len, 0);
	len+=DoDataPacket(buffer, oBuf+len, 0);
	len+=DoEndPacket(oBuf+len, 2, 0);
	return len;
}