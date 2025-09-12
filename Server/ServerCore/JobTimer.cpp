#include "pch.h"
#include "JobTimer.h"
#include "JobQueue.h"
#include "ObjectPool.h"
void JobTimer::Reserve(uint64_t tickAfter, std::weak_ptr<JobQueue> owner, std::shared_ptr<Job> job)
{
	const uint64_t executeTick = PocoTimeUtil::GetTick64() + tickAfter;
	JobData* jobData = ObjectPool<JobData>::GetInstance().New(owner, job);

	mTimerQueue.push(std::move(TimerItem{ executeTick, jobData }));
}

void JobTimer::Distribute(uint64_t now)
{
	// �� ���� 1 �����常 ���
	if (mIsDistributing.exchange(true) == true)
		return;

	std::vector<TimerItem> items;

	{
		TimerItem timerItem;
		while (mTimerQueue.try_pop(timerItem) == true)
		{
			if (now < timerItem.executeTick)
			{
				mTimerQueue.push(std::move(timerItem));
				break;
			}
			items.push_back(timerItem);
		}
	}

	for (TimerItem& item : items)
	{	
		if (std::shared_ptr<JobQueue> owner = item.jobData->owner.lock())
			owner->Push(item.jobData->job);
		ObjectPool<JobData>::GetInstance().Delete(item.jobData);
	}

	// �������� Ǯ���ش�
	mIsDistributing.store(false);
}

void JobTimer::Clear()
{

	TimerItem timerItem;
	while (mTimerQueue.try_pop(timerItem) == true)
	{

	}
}