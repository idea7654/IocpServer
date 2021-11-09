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

private:

};