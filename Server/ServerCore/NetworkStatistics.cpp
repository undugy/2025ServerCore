#include "pch.h"
#include "NetworkStatistics.h"
#include "Job.h"
#include "DBService.h"
#include "RegisteredBufferPool.h"
void NetworkStatistics::PrintStatistics()
{
	if (mIsRunning.load() == false)
		return;

	auto jobAllocCount = ObjectPool<Job>::GetInstance().GetAllocCount();
	auto jobUsingCount = ObjectPool<Job>::GetInstance().GetUsingCount();

	// Print statistics
	VIEW_INFO("Job Alloc Count: {}", jobAllocCount);
	VIEW_INFO("Job Using Count: {}", jobUsingCount);

	auto dbAllocCount = ObjectPool<DBQueueData>::GetInstance().GetAllocCount();
	auto dbUsingCount = ObjectPool<DBQueueData>::GetInstance().GetUsingCount();

	VIEW_INFO("DB Alloc Count: {}", dbAllocCount);
	VIEW_INFO("DB Using Count: {}", dbUsingCount);

#ifdef VERSION_RIO
	auto recvBuffAllocCount = RegisteredBufferPool<RECV_BUFFER_SIZE>::GetInstance().GetFreeCount();
	auto recvBuffUsingCount = RegisteredBufferPool<RECV_BUFFER_SIZE>::GetInstance().GetUsingCount();

	VIEW_INFO("Recv Buffer Alloc Count: {}", recvBuffAllocCount);
	VIEW_INFO("Recv Buffer Using Count: {}", recvBuffUsingCount);

	auto sendBuffAllocCount = RegisteredBufferPool<SEND_BUFFER_SIZE>::GetInstance().GetFreeCount();
	auto sendBuffUsingCount = RegisteredBufferPool<SEND_BUFFER_SIZE>::GetInstance().GetUsingCount();
	VIEW_INFO("Send Buffer Alloc Count: {}", sendBuffAllocCount);
	VIEW_INFO("Send Buffer Using Count: {}", sendBuffUsingCount);
#endif
}
