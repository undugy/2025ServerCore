#pragma once
#include"RefSingleton.h"
class ThreadManager : public RefSingleton<ThreadManager>
{
public:
	ThreadManager();
	~ThreadManager();

	void PushThread(std::function<void(void)> fn);
	void SetThreadID();

private:

	std::vector<std::jthread> _threads;
};

