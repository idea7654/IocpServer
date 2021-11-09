#include "MainIocp.h"

int main()
{
	MainIocp server;
	if (server.InitSocket())
	{
		server.StartServer();
	}

	return 0;
}