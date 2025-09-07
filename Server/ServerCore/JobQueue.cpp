#include "pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void JobQueue::Push(std::shared_ptr<Job> job, bool pushOnly /*= false*/)
{
	const int32_t prevCount = mJobCount.fetch_add(1);
	mJobQueue.enqueue(job);

	// ù��° Job�� ���� �����尡 ������� ���
	if (prevCount == 0)
	{
		// �̹� �������� JobQueue�� ������ ����
		if (LCurrentJobQueue == nullptr && pushOnly == false)
		{
			Execute();
		}
		else
		{
			// ���� �ִ� �ٸ� �����尡 �����ϵ��� GlobalQueue�� �ѱ��
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

		// ���� �ϰ��� 0����� ����
		if (mJobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

		const uint64_t now = PocoTimeUtil::GetTick64();
		if (now >= LEndTickCount) // �ʹ� ���� ��� ���� ����
		{
			LCurrentJobQueue = nullptr;
			GlobalQueue::GetInstance().Push(shared_from_this());
			break;
		}
	}
}
