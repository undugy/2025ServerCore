#include "pch.h"
#include "ThreadManager.h"
#include "ThreadLocal.h"
#include "GlobalQueue.h"
#include "JobQueue.h"
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

void ThreadManager::DoGlobalJobs()
{
	while (true)
	{
		int64_t now = PocoTimeUtil::GetTick64();
		if (now > LEndTickCount)
			break;

		std::shared_ptr<JobQueue> jobQueue = GlobalQueue::GetInstance().Pop();
		if (jobQueue == nullptr)
			break;

		jobQueue->Execute();
	}
}

void ThreadManager::DistributeJobs()
{
	const int64_t now = PocoTimeUtil::GetTick64();
	JobTimer::GetInstance().Distribute(now);
}
