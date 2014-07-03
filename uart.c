#include <windows.h>
#include "system.h"
#include "uart.h"

HANDLE uart=INVALID_HANDLE_VALUE;

void uart_Initialize(void)
{
  DCB dcb;
  if ((uart=CreateFile("COM1",GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,0))==INVALID_HANDLE_VALUE)
  {
    MessageBox(0,"打开串口错误!","错误信息",MB_OK);
    return;
  }
  
  SetupComm(uart,8192,8192);
  GetCommState(uart,&dcb);
  dcb.BaudRate=CBR_9600; //CBR_57600;
  dcb.ByteSize=8;
  dcb.Parity=NOPARITY;
  dcb.StopBits=ONESTOPBIT;
  SetCommState(uart,&dcb);
}

void uart_Destroy(void)
{
  if (uart!=INVALID_HANDLE_VALUE) CloseHandle(uart);
}

DWORD uart_Write(BYTE *buffer,DWORD size, PDeviceInfo dev)
{
  DWORD dwSize;
  if (uart==INVALID_HANDLE_VALUE) return 0;

  WriteFile(uart,buffer,size,&dwSize,NULL);
  return dwSize;
}

DWORD uart_Read(BYTE *buffer,DWORD size,PDeviceInfo device)
{
  COMSTAT state;
  DWORD dwRead;

  if (uart==INVALID_HANDLE_VALUE) return 0;

  memset(device,0,sizeof(TDeviceInfo));
  ClearCommError(uart,&dwRead,&state);
  if (state.cbInQue>0) 
  {
    if (state.cbInQue>size) ReadFile(uart,buffer,size,&dwRead,NULL);
    else ReadFile(uart,buffer,state.cbInQue,&dwRead,NULL);
  }
  else dwRead=0;

  device->dev_id=dev_uart;
  return dwRead;
}