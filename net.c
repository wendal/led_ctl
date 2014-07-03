#include <windows.h>
#include "system.h"
#include "net.h"

SOCKET sock=INVALID_SOCKET;

void net_Initialize(int port)
{
	struct sockaddr_in addr;

	WSADATA Data;
	int err;

	sock=INVALID_SOCKET;
	err = WSAStartup(MAKEWORD(1,1), &Data);
	if (err!=0) return;

	sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock!=INVALID_SOCKET)
	{
		addr.sin_family=AF_INET;
		addr.sin_port=htons((WORD)port);
		addr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);

		if (bind(sock, (struct sockaddr*)(&addr), sizeof(struct sockaddr_in))==SOCKET_ERROR) 
		{
			closesocket(sock);
			sock=INVALID_SOCKET;
		}
	}
}

void net_Destroy()
{
	if (sock!=INVALID_SOCKET) closesocket(sock);
	WSACleanup();
}

int net_Write(char* buffer, DWORD size, char* ip, WORD port)
{
	struct sockaddr_in addr;
	if (sock!=INVALID_SOCKET)
	{
		addr.sin_family=AF_INET;
		addr.sin_port=htons(port);
		addr.sin_addr.S_un.S_addr=inet_addr(ip);
		return sendto(sock, (const char*)buffer, size, 0, (struct sockaddr*)(&addr), sizeof(struct sockaddr_in));
	}
	return 0;
}

int net_Read(char* buffer, DWORD size, char* ip, WORD* port)
{
	fd_set readfds;
	struct timeval timeout;
	int dwSize, fromlen;
	struct sockaddr_in addr;

	if (sock!=INVALID_SOCKET)
	{
		readfds.fd_count=0;
		if (readfds.fd_count<FD_SETSIZE)
		{
			readfds.fd_array[readfds.fd_count]=sock;
			readfds.fd_count++;
		}
		timeout.tv_sec=0;
		timeout.tv_usec=1;
		if (select(0, &readfds, NULL, NULL, &timeout))
		{
			if (FD_ISSET(sock, &readfds))
			{
				fromlen=sizeof(struct sockaddr_in);
				dwSize=recvfrom(sock, (char*)buffer, size, 0, (struct sockaddr*)(&addr), &fromlen);
				if (dwSize!=SOCKET_ERROR)
				{
					*port=htons(addr.sin_port);
					strcpy(ip, inet_ntoa(addr.sin_addr));
					return dwSize;
				}
			}
		}
	}
	return 0;
}