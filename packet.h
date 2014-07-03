#ifndef __packet__
#define __packet__

#include "system.h"
#include "winsock.h"

#define  PKC_BEGIN            0
#define  PKC_END              1
#define  PKC_DATA             2
#define  PKC_RESPOND          3
#define  PKC_ADJUST_TIME      6
#define  PKC_GET_POWER        9
#define  PKC_SET_POWER       10
#define  PKC_GET_BRIGHT      11
#define  PKC_SET_BRIGHT      12
#define  PKC_GET_FILLCOLOR   14
#define  PKC_SET_FILLCOLOR   15
#define  PKC_GET_LEAF        65
#define  PKC_SET_LEAF        66

#define  PKP_LEADER  0x55
#define  PKP_SUFFIX  0xAA
#define  PKP_FILLCH  0xBB
#define  PKP_LENGTH  512
#define  PKP_MAX_LENGTH PKP_LENGTH*2+sizeof(TPKG_Header)*2

#define  FILLCOLOR_BUFFER_LENGTH  32


typedef struct PKG_HEADER{
  WORD   Command;
  BYTE   srcAddr;
  BYTE   dstAddr;
  DWORD  SerialNo;
}TPKG_Header, *PPKG_Header;

typedef struct PKG_RESPOND{
  TPKG_Header  Header;
  WORD         Command;
  WORD         Result;
}TPKG_Respond, *PPKG_Respond;

extern WORD CreateCRC(BYTE *buffer, DWORD size);
extern WORD PackEx(BYTE *cbuf,WORD csize,BYTE *ibuf,BYTE *obuf,WORD size);
extern WORD Pack(BYTE *ibuf,BYTE *obuf,WORD size);
extern int dePack(BYTE *ibuf,BYTE *obuf,int size);

extern PRoot   root;
extern PLeaf   leaf;
extern PString string;

DWORD BeginPacket(BYTE* obuf, BYTE dstAddr);
DWORD DataPacket(BYTE* obuf, BYTE dstAddr, char* str1, char* str2);
DWORD EndPacket(BYTE* obuf, BYTE dstAddr);

SOCKET InitSocket(int iport);
int CloseSocket(SOCKET s);
int SocketWrite(SOCKET s, char* buffer, DWORD size, char* ip, WORD port);
int SocketRead(SOCKET s, char* buffer, DWORD size, char* ip, WORD* port);


#endif