#ifndef __net__
#define __net__

#include "system.h"

extern  void net_Initialize(int port);
extern  int net_Read(char* buffer, DWORD size, char* ip, WORD* port);
extern  int net_Write(char* buffer, DWORD size, char* ip, WORD port);
extern  void net_Destroy(void);

#endif