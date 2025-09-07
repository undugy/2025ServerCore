#pragma once
#include "RefSingleton.h"

class Job;
class JobData;

class JobData
{
public:
	JobData(std::weak_ptr<JobQueue> owner, std::shared_ptr<Job> job) : owner(owner), job(job)
	{

	}

	std::weak_ptr<JobQueue>	owner;
	std::shared_ptr<Job>	job;
};

class TimerItem
{
public:
	bool operator<(const TimerItem& other) const
	{
		return executeTick < other.executeTick;
	}

	uint64_t executeTick = 0;
	JobData* jobData;
};

/// <summary>
/// 당장 실행이 아닌 일정 시간 뒤에 처리할 작업을 처리하기 위함
/// </summary>
class JobTimer : public RefSingleton<JobTimer>
{
public:

	void			Reserve(uint64_t tickAfter, std::weak_ptr<JobQueue> owner, std::shared_ptr<Job> job);	

	void			Distribute(uint64_t now);
	void			Clear();

private:
	concurrency::concurrent_priority_queue<TimerItem>	mTimerQueue;
	std::atomic_bool				mIsDistributing = false;
};
