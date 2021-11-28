#include "MainIocp.h"
#include "IocpBase.h"
#include <sstream>
#pragma warning(disable:4996)

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
			string newStr = pSocketInfo->dataBuf.buf;
			auto strArr = split(newStr, ' ');
			if (strArr[0] == "Position")
			{
				BroadCast(pSocketInfo->dataBuf.buf);
			}
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
	int nResult;
	DWORD	sendBytes;
	DWORD	dwFlags = 0;

	nResult = WSASend(
		pSocket->socket,
		&(pSocket->dataBuf),
		1,
		&sendBytes,
		dwFlags,
		NULL,
		NULL
	);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		cout << "[ERROR] WSASend ����" << endl;
	}
}

void MainIocp::BroadCast(string msg)
{
	for (auto &i : mClients)
	{
		int nResult;
		DWORD	sendBytes;
		DWORD	dwFlags = 0;

		memcpy(i->messageBuffer, msg.c_str(), sizeof(msg));
		i->dataBuf.buf = i->messageBuffer;
		i->dataBuf.len = sizeof(msg);

		nResult = WSASend(
			i->socket,
			&(i->dataBuf),
			1,
			&sendBytes,
			dwFlags,
			NULL,
			NULL
		);

		if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			cout << "[ERROR] WSASend ����" << endl;
		}
	}
}

void MainIocp::ClientConnect(SOCKETINFO * pSocket)
{	
	BroadCast("YourID " + to_string(pSocket->ID));
	cout << pSocket->ID << endl;
}

vector<string> MainIocp::split(string str, char Delimiter) {
	istringstream iss(str);             // istringstream�� str�� ��´�.
	string buffer;                      // �����ڸ� �������� ����� ���ڿ��� ������� ����

	vector<string> result;

	// istringstream�� istream�� ��ӹ����Ƿ� getline�� ����� �� �ִ�.
	while (getline(iss, buffer, Delimiter)) {
		result.push_back(buffer);               // ����� ���ڿ��� vector�� ����
	}

	return result;
}