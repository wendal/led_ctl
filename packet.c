#include "system.h"
#include "packet.h"
#include "winsock.h"

BYTE  buffer[1024];    //����һ���ڴ���

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
  leaf->time=100000000;         //�ٶ���ҳ��ʾ60��
  leaf->Reserved=0;

  root->count++;
  root->size+=leaf->size;
}

//���������ʾ
//  x,y,cx,cy	������ʾ�����Ͻ�X���꣬���Ͻ�Y���꣬��ȣ��߶�
//  str			��ʾ����������
//  font		��ʾ�����壬=0Ϊ16����=1Ϊ24����
//  color		������ʾ����ɫ
void AddString(int x, int y, int cx, int cy, char* str, int font, unsigned long color)
{
  int w;
  if (root==NULL || leaf==NULL) return;

  string=(PString)((DWORD)leaf+leaf->size);

  string->object.size=sizeof(TString)+strlen(str)+1;  //��һ��Ϊ��������ֹ��(0)��λ��
  if (string->object.size & 3)  //�ж��������ݳ����Ƿ�4�ֽڶ���
  {
    string->object.size&=~3;    //�����������ж���
    string->object.size+=4;
  }
  
  string->object.id=OBJECT_STRING;
  string->object.rect.left=x;
  string->object.rect.top=y;
  string->object.rect.right=x+cx;
  string->object.rect.bottom=y+cy;
  string->object.transparent=1;
  string->fontset=0;                   // 0 = 16����,  1 = 24����
  string->color=color;            //��ɫ(ȷ�����κ���ʾ���϶�����ʾ, ��: �������ϲ�����ʾ��ɫ)
  if (font==0) w=8; else w=12;
  if ((strlen(str)*w)>(WORD)cx) string->method=2;                    //1=������ʾ����ѡ����ֶ�����ʽ
  else string->method=2;
  string->speed=1;                     //1���ٶ�

  memcpy((void*)((DWORD)string+sizeof(TString)), str, strlen(str)+1);

  leaf->count++;
  leaf->size+=string->object.size;
  root->size+=string->object.size;
}

//������ʼ������
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

//�������ݰ�����
DWORD DataPacket(BYTE* obuf, BYTE dstAddr, char* str1, char* str2)
{
	TPKG_Header header;
	DWORD len;
	header.Command=PKC_DATA;
	header.dstAddr=dstAddr;
	header.srcAddr=0;
	header.SerialNo=1;
	//ϵͳ��֧�־ֲ����£�ÿ�η��͵Ľ�Ŀ�Ḳ�ǵ�ԭ�н�Ŀ��
	//��ˣ�ÿ�ζ���Ҫ��MakeRoot��ʼ���á�
	//���Ҫ��ʾ�������֣���ôÿ���������ֶ���Ҫ��ӣ���������ʾ
	MakeRoot();
	AddLeaf();
	AddString(0, 0, 128, 16, str1, 0, 0x000000FF);	//16���󣬺�ɫ
	AddString(0, 16, 128, 16, str2, 0, 0x0000FF00);	//
	len=PackEx((BYTE*)&header, sizeof(TPKG_Header), (BYTE*)root, obuf, (WORD)(root->size));
	return len;
}

//���ɽ���������
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
