#pragma once

#include "Types.h"
#include "CorePch.h"
class IocpBase
{
public:
	IocpBase();
	~IocpBase();

	bool InitSocket();

	virtual void StartServer();
	virtual bool CreateWorkerThread();
	virtual void WorkerThread();
	void Send(SOCKETINFO *pSocket);
	void Recv(SOCKETINFO *pSocket);

protected:
	SOCKETINFO		*mSocketInfo;
	SOCKET			mListenSocket;
	HANDLE			mIOCP;
	bool			mAccept;
	bool			mWorkerThread;
	vector<thread>	mThreadPool;
	int32			mThreadCount;
};