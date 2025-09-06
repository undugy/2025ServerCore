#include "pch.h"
#include "ThreadManager.h"
#include "ThreadLocal.h"
ThreadManager::ThreadManager()
{
	SetThreadID();
}

ThreadManager::~ThreadManager()
{

}

void ThreadManager::PushThread(std::function<void(void)> fn)
{

	_threads.push_back(std::jthread([=]()
		{
			SetThreadID();
			fn();
		}));
}

void ThreadManager::SetThreadID()
{
	static std::atomic_int threadId = 0;
	LThreadId = ++threadId;
}
