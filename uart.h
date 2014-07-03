#ifndef __uart__
#define __uart__

#include "system.h"

#define dev_uart  0
#define dev_udp   1
#define dev_485   2

typedef struct DEVICEINFO{
  WORD  dev_id;
  WORD  port;
  DWORD ip;
}TDeviceInfo, *PDeviceInfo;

extern  void uart_Initialize(void);
extern  DWORD uart_Read(BYTE *buffer,DWORD size,PDeviceInfo device);
extern  DWORD uart_Write(BYTE *buffer,DWORD size, PDeviceInfo dev);
extern  void uart_Destroy(void);

#endif