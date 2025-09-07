#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void JobQueue::Push(std::shared_ptr<Job> job, bool pushOnly /*= false*/)
{
	const int32_t prevCount = mJobCount.fetch_add(1);
	mJobQueue.enqueue(job);

	// 첫번째 Job을 넣은 쓰레드가 실행까지 담당
	if (prevCount == 0)
	{
		// 이미 실행중인 JobQueue가 없으면 실행
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			// 여유 있는 다른 쓰레드가 실행하도록 GlobalQueue에 넘긴다
			GlobalQueue::GetInstance().Push(shared_from_this());
		}
	}
}

void JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		std::vector<std::shared_ptr<Job>> jobs(BULK_DEQUEUE_JOB_COUNT);
		size_t n = mJobQueue.try_dequeue_bulk(jobs.begin(), BULK_DEQUEUE_JOB_COUNT);
		int jobCount = static_cast<int>(n);
		for (int i = 0; i < jobCount; i++)
			jobs[i]->Execute();

		// 남은 일감이 0개라면 종료
		if (mJobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64_t now = PocoTimeUtil::GetTick64();
		if (now >= LEndTickCount) // 너무 오래 잡고 있지 말자
		{
			LCurrentJobQueue = nullptr;
			GlobalQueue::GetInstance().Push(shared_from_this());
			break;
		}
	}
}
