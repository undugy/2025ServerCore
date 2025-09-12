#pragma once
#include"JobQueue.h"
class ListenerService;
class TimerJobQueue : public JobQueue
{
public:
	void UpdateTime();

};
class ServerApp
{
public:
	bool Init();
	bool Update();

private:
	bool InitListener();
	bool InitWorkerThread(std::shared_ptr<ListenerService>& service);
	bool InitGameDB();
	bool InitObjectPool();
};

