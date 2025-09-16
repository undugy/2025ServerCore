#include "pch.h"
#include "NetworkStatistics.h"
#include "Job.h"
#include "DBService.h"
#include "RegisteredBufferPool.h"
NetworkStatistics::NetworkStatistics()
{
	PdhOpenQuery(NULL, NULL, &_cpuQuery);
	PdhAddCounterW(_cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &_cpuTotal);
}
void NetworkStatistics::PrintStatistics()
{
	if (mIsRunning.load() == false)
		return;
	VIEW_WRITE_INFO("CPU ({:.2f}), MEOMORY ({:.2f})MB", GetCPUUsage(), GetMemoryUsage_MB());
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

const double NetworkStatistics::GetCPUUsage()
{
	PdhCollectQueryData(_cpuQuery);
	PdhGetFormattedCounterValue(_cpuTotal, PDH_FMT_DOUBLE, NULL, &_counterVal);
	return _counterVal.doubleValue;
}

const uint64_t NetworkStatistics::GetMemoryUsage_Byte()
{
	PROCESS_MEMORY_COUNTERS_EX pmc;
	if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc)))
		return pmc.PrivateUsage;

	return 0;
}

const double NetworkStatistics::GetMemoryUsage_KB()
{
	return static_cast<double>(GetMemoryUsage_Byte()) / 1024.0;
}

const double NetworkStatistics::GetMemoryUsage_MB()
{
	return GetMemoryUsage_KB() / 1024.0;
}
