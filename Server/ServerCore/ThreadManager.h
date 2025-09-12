#pragma once
#include"RefSingleton.h"
class ThreadManager : public RefSingleton<ThreadManager>
{
public:
	ThreadManager();
	~ThreadManager();

	void PushThread(std::function<void(void)> fn);
	void SetThreadID();

	void DoGlobalJobs();
	void DistributeJobs();
private:

	std::vector<std::jthread> _threads;
};

