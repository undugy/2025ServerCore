#include "pch.h"
#include "GlobalQueue.h"

void GlobalQueue::Push(std::shared_ptr<JobQueue> jobQueue)
{
	mJobQueues.enqueue(jobQueue);
}

std::shared_ptr<JobQueue> GlobalQueue::Pop()
{
	std::shared_ptr<JobQueue> jobQueue = nullptr;
	while (false == mJobQueues.try_dequeue(jobQueue)){};
	return jobQueue;
}
