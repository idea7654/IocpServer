#pragma once

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

using BYTE = unsigned char;
using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

using namespace std;

#define MAX_BUFFER 1024
#define SERVER_PORT 9706

struct SOCKETINFO
{
	WSAOVERLAPPED	overlapped;
	WSABUF			dataBuf;
	SOCKET			socket;
	char			messageBuffer[MAX_BUFFER];
	int32			recvBytes;
	int32			sendBytes;
};