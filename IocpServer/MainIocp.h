#pragma once

#include "IocpBase.h"

class MainIocp : public IocpBase
{
public:
	MainIocp();
	~MainIocp();

	virtual void StartServer() override;
	virtual bool CreateWorkerThread() override;
	virtual void WorkerThread() override;
	static void Send(SOCKETINFO *pSocket);
	void BroadCast(string msg);
	virtual void ClientConnect(SOCKETINFO *pSocket) override;
private:
	vector<string> split(string str, char Delimiter);
};