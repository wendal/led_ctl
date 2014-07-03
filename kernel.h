#ifndef __kernel__
#define __kernel__

#include "system.h"

void DoMakeRoot(PRoot root);

void DoAddLeaf(PRoot root, DWORD time);

//添加内码文字显示
//  rect		文字显示的区域
//  str			显示的文字内容
//  font		显示的字体，=0为16点阵，=1为24点阵
//  method		动作方式
//  color		文字显示的颜色
void DoAddString(PRoot root, RECT* rect, char* str, int font, int method, unsigned long color);

//添加图片显示
//  rect		文字显示的区域
//  method      动作方式
//  width       图片宽度
//  height      图片高度
//  data        图片点阵数据
void DoAddWindow(PRoot root, RECT* rect, int method, WORD width, WORD height, BYTE* data);

//添加Windows字体文字显示
//  rect		文字显示的区域
//  method      动作方式
//  fontname	字体名称
//  fontsize	字体大小
//  fontcolor	字体颜色
//  str			显示的文字内容
void DoAddText(PRoot root, RECT* rect, int method, char* fontname, DWORD fontsize, DWORD fontcolor, char* str);

//添加字库文件文字显示
//  rect		文字显示的区域
//  str			显示的文字内容
//  fontfile	字库文件
//  method      动作方式
//  color		文字显示的颜色
void DoAddFontSetText(PRoot root, RECT* rect, char* str, char* fontfile, int method, unsigned long color);

//生成起始包数据
DWORD DoBeginPacket(BYTE* obuf, BYTE dstAddr);
//生成所有数据包
DWORD DoDataPacket(BYTE* ibuf, BYTE* obuf, BYTE dstAddr);
//获取数据包数量
DWORD GetDataPacketCount(BYTE* ibuf);
//生成数据包
DWORD DoOneDataPacket(BYTE* ibuf, DWORD index, BYTE* obuf, BYTE dstAddr);
//生成结束包数据
DWORD DoEndPacket(BYTE* obuf, DWORD index, BYTE dstAddr);

#endif
