#include "system.h"
#include "packet.h"
#include "winsock.h"

BYTE  buffer[1024];    //分配一段内存区

PRoot   root=NULL;
PLeaf   leaf=NULL;
PString string=NULL;

void MakeRoot(void)
{
  root=(PRoot)buffer;

  root->id=ROOT_PLAY;
  root->count=0;
  root->size=sizeof(TRoot);
  root->type=SCREEN_COLOR; //SCREEN_COLOR;
  root->Reserved=0;
}

void AddLeaf(void)
{
  if (root==NULL) return;

  leaf=(PLeaf)((DWORD)root+root->size);

  leaf->id=OBJECT_LEAF;
  leaf->count=0;
  leaf->size=sizeof(TLeaf);
  leaf->time=100000000;         //假定本页显示60秒
  leaf->Reserved=0;

  root->count++;
  root->size+=leaf->size;
}

//添加文字显示
//  x,y,cx,cy	文字显示的左上角X坐标，左上角Y坐标，宽度，高度
//  str			显示的文字内容
//  font		显示的字体，=0为16点阵，=1为24点阵
//  color		文字显示的颜色
void AddString(int x, int y, int cx, int cy, char* str, int font, unsigned long color)
{
  int w;
  if (root==NULL || leaf==NULL) return;

  string=(PString)((DWORD)leaf+leaf->size);

  string->object.size=sizeof(TString)+strlen(str)+1;  //加一是为了留出终止符(0)的位置
  if (string->object.size & 3)  //判断整个数据长度是否4字节对齐
  {
    string->object.size&=~3;    //如果不是则进行对齐
    string->object.size+=4;
  }
  
  string->object.id=OBJECT_STRING;
  string->object.rect.left=x;
  string->object.rect.top=y;
  string->object.rect.right=x+cx;
  string->object.rect.bottom=y+cy;
  string->object.transparent=1;
  string->fontset=0;                   // 0 = 16点阵,  1 = 24点阵
  string->color=color;            //黄色(确保在任何显示屏上都能显示, 如: 单红屏上不能显示绿色)
  if (font==0) w=8; else w=12;
  if ((strlen(str)*w)>(WORD)cx) string->method=2;                    //1=立即显示，可选择多种动作方式
  else string->method=2;
  string->speed=1;                     //1级速度

  memcpy((void*)((DWORD)string+sizeof(TString)), str, strlen(str)+1);

  leaf->count++;
  leaf->size+=string->object.size;
  root->size+=string->object.size;
}

//生成起始包数据
DWORD BeginPacket(BYTE* obuf, BYTE dstAddr)
{
	TPKG_Header header;
	DWORD len;
	header.Command=PKC_BEGIN;
	header.dstAddr=dstAddr;
	header.srcAddr=0;
	header.SerialNo=0;
	len=Pack((BYTE*)&header,obuf,sizeof(TPKG_Header));
	return len;
}

//生成数据包数据
DWORD DataPacket(BYTE* obuf, BYTE dstAddr, char* str1, char* str2)
{
	TPKG_Header header;
	DWORD len;
	header.Command=PKC_DATA;
	header.dstAddr=dstAddr;
	header.srcAddr=0;
	header.SerialNo=1;
	//系统不支持局部更新，每次发送的节目会覆盖掉原有节目。
	//因此，每次都需要从MakeRoot开始调用。
	//如果要显示两行文字，那么每次两行文字都需要添加，如下面所示
	MakeRoot();
	AddLeaf();
	AddString(0, 0, 128, 16, str1, 0, 0x000000FF);	//16点阵，红色
	AddString(0, 16, 128, 16, str2, 0, 0x0000FF00);	//
	len=PackEx((BYTE*)&header, sizeof(TPKG_Header), (BYTE*)root, obuf, (WORD)(root->size));
	return len;
}

//生成结束包数据
DWORD EndPacket(BYTE* obuf, BYTE dstAddr)
{
	TPKG_Header header;
	DWORD len;
	header.Command=PKC_END;
	header.dstAddr=dstAddr;
	header.srcAddr=0;
	header.SerialNo=2;
	len=Pack((BYTE*)&header,obuf,sizeof(TPKG_Header));
	return len;
}
