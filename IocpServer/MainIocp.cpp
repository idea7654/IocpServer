#include "MainIocp.h"
#include "IocpBase.h"

uint32 WINAPI CallWorkerThread(LPVOID p)
{
	MainIocp *pOverlappedEvent = (MainIocp*)p;
	pOverlappedEvent->WorkerThread();
	return 0;
}

MainIocp::MainIocp()
{
	mWorkerThread = true;
	mAccept = true;
}

MainIocp::~MainIocp()
{
	WSACleanup();
	if (mSocketInfo)
	{
		delete[] mSocketInfo;
		mSocketInfo = NULL;
	}

	mThreadPool.empty();
}

void MainIocp::StartServer()
{
	IocpBase::StartServer();
}

bool MainIocp::CreateWorkerThread()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	mThreadCount = sysInfo.dwNumberOfProcessors * 2;

	for (int32 i = 0; i < mThreadCount; i++)
	{
		mThreadPool.emplace_back(thread(&CallWorkerThread, this));
	}

	return true;
}

void MainIocp::WorkerThread()
{
	bool bResult;
	DWORD recvBytes;
	DWORD sendBytes;
	SOCKETINFO *pCompletionKey;
	SOCKETINFO *pSocketInfo;
	DWORD dwFlags = 0;

	while (mWorkerThread)
	{
		bResult = ::GetQueuedCompletionStatus(mIOCP, &recvBytes, (PULONG_PTR)&pCompletionKey, (LPOVERLAPPED*)&pSocketInfo, INFINITE);

		if (!bResult && recvBytes == 0)
		{
			cout << "socket disconnect" << endl;
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}

		pSocketInfo->dataBuf.len = recvBytes;

		if (recvBytes == 0)
		{
			closesocket(pSocketInfo->socket);
			free(pSocketInfo);
			continue;
		}

		try
		{
			cout << pSocketInfo->dataBuf.buf << endl;
		}
		catch (const exception &e)
		{
			cout << "Exception Error" << endl;
		}

		Recv(pSocketInfo);
	}
}

void MainIocp::Send(SOCKETINFO * pSocket)
{
}