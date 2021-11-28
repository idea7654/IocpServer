#include "IocpBase.h"

IocpBase::IocpBase()
{
	mWorkerThread = true;
	mAccept = true;
}

IocpBase::~IocpBase()
{
	WSACleanup();
	if (mSocketInfo)
	{
		delete[] mSocketInfo;
		mSocketInfo = NULL;
	}

	mThreadPool.empty();
}

bool IocpBase::InitSocket()
{
	WSADATA wsaData;
	int nResult;
	nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (nResult != 0)
	{
		std::cout << "Init Error" << std::endl;
		return false;
	}

	mListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (mListenSocket == INVALID_SOCKET)
	{
		std::cout << "WSASocket Error" << std::endl;
		return false;
	}

	bool NoDelay = true;
	setsockopt(mListenSocket, IPPROTO_TCP, TCP_NODELAY, (const char FAR*) &NoDelay, sizeof(NoDelay));

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	nResult = ::bind(mListenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));

	if (nResult == SOCKET_ERROR)
	{
		cout << "bind Error" << endl;
		closesocket(mListenSocket);
		WSACleanup();
		return false;
	}

	nResult = listen(mListenSocket, 5);
	if (nResult == SOCKET_ERROR)
	{
		cout << "Listen Error" << endl;
		closesocket(mListenSocket);
		WSACleanup();
		return false;
	}

	return true;
}

void IocpBase::StartServer()
{
	int nResult;

	SOCKADDR_IN clientAddr;
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKET clientSocket;
	DWORD recvBytes;
	DWORD flags;

	mIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (!CreateWorkerThread()) return;

	cout << "Server Start..." << endl;

	while (mAccept)
	{
		clientSocket = WSAAccept(mListenSocket, (struct sockaddr*)&clientAddr, &addrLen, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Accept Error" << endl;
			return;
		}

		mSocketInfo = new SOCKETINFO();
		mSocketInfo->socket = clientSocket;
		mSocketInfo->recvBytes = 0;
		mSocketInfo->sendBytes = 0;
		mSocketInfo->dataBuf.len = MAX_BUFFER;
		mSocketInfo->dataBuf.buf = mSocketInfo->messageBuffer;
		flags = 0;

		mIOCP = CreateIoCompletionPort((HANDLE)clientSocket, mIOCP, (ULONG_PTR)mSocketInfo, 0);

		nResult = WSARecv(mSocketInfo->socket, &mSocketInfo->dataBuf, 1, &recvBytes, &flags, &(mSocketInfo->overlapped), NULL);

		mSocketInfo->ID = ID;
		mClients.push_back(mSocketInfo);
		ID++;
		if (ID >= 3)
			ID = 1;

		if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
		{
			cout << "IO Pending Error" << endl;
			return;
		}

		ClientConnect(mSocketInfo);
	}
}

bool IocpBase::CreateWorkerThread()
{
	return true;
}

void IocpBase::WorkerThread()
{
}

void IocpBase::Send(SOCKETINFO * pSocket)
{

}

void IocpBase::Recv(SOCKETINFO * pSocket)
{
	int32 nResult;
	DWORD dwFlags = 0;

	//memset(&(pSocket->overlapped), NULL, sizeof(OVERLAPPED));
	//memset(pSocket->messageBuffer, NULL, MAX_BUFFER);
	ZeroMemory(&(pSocket->overlapped), sizeof(OVERLAPPED));
	ZeroMemory(pSocket->messageBuffer, MAX_BUFFER);
	pSocket->dataBuf.len = MAX_BUFFER;
	pSocket->dataBuf.buf = pSocket->messageBuffer;
	pSocket->recvBytes = 0;
	pSocket->sendBytes = 0;

	dwFlags = 0;

	nResult = WSARecv(pSocket->socket, &(pSocket->dataBuf), 1, (LPDWORD)&pSocket, &dwFlags, (LPWSAOVERLAPPED)&(pSocket->overlapped), NULL);

	if (nResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		cout << "WSARecv Error" << endl;
	}
}

void IocpBase::ClientConnect(SOCKETINFO * pSocket)
{
}
