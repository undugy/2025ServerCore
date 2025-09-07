#pragma once
#include "RefSingleton.h"

class JobQueue;
class DBData;

/// <summary>
/// DB�۾� ����, ���� �� ��Ʈ��ũ ��Ŷ �۾� ť
/// �� �����忡 �ʹ� ���� �۾��� ���� ��츦 ���� ť, DB�� �ε�뷱���� ����
/// </summary>
class GlobalQueue : public RefSingleton<GlobalQueue>
{
public:
	void									Push(std::shared_ptr<JobQueue> jobQueue);
	std::shared_ptr<JobQueue>				Pop();

private:
	moodycamel::ConcurrentQueue<std::shared_ptr<JobQueue>> mJobQueues;
};
