#ifndef __kernel__
#define __kernel__

#include "system.h"

void DoMakeRoot(PRoot root);

void DoAddLeaf(PRoot root, DWORD time);

//�������������ʾ
//  rect		������ʾ������
//  str			��ʾ����������
//  font		��ʾ�����壬=0Ϊ16����=1Ϊ24����
//  method		������ʽ
//  color		������ʾ����ɫ
void DoAddString(PRoot root, RECT* rect, char* str, int font, int method, unsigned long color);

//���ͼƬ��ʾ
//  rect		������ʾ������
//  method      ������ʽ
//  width       ͼƬ���
//  height      ͼƬ�߶�
//  data        ͼƬ��������
void DoAddWindow(PRoot root, RECT* rect, int method, WORD width, WORD height, BYTE* data);

//���Windows����������ʾ
//  rect		������ʾ������
//  method      ������ʽ
//  fontname	��������
//  fontsize	�����С
//  fontcolor	������ɫ
//  str			��ʾ����������
void DoAddText(PRoot root, RECT* rect, int method, char* fontname, DWORD fontsize, DWORD fontcolor, char* str);

//����ֿ��ļ�������ʾ
//  rect		������ʾ������
//  str			��ʾ����������
//  fontfile	�ֿ��ļ�
//  method      ������ʽ
//  color		������ʾ����ɫ
void DoAddFontSetText(PRoot root, RECT* rect, char* str, char* fontfile, int method, unsigned long color);

//������ʼ������
DWORD DoBeginPacket(BYTE* obuf, BYTE dstAddr);
//�����������ݰ�
DWORD DoDataPacket(BYTE* ibuf, BYTE* obuf, BYTE dstAddr);
//��ȡ���ݰ�����
DWORD GetDataPacketCount(BYTE* ibuf);
//�������ݰ�
DWORD DoOneDataPacket(BYTE* ibuf, DWORD index, BYTE* obuf, BYTE dstAddr);
//���ɽ���������
DWORD DoEndPacket(BYTE* obuf, DWORD index, BYTE dstAddr);

#endif
