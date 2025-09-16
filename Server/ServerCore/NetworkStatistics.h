#pragma once
#include "pdh.h"
#pragma comment(lib, "pdh.lib")
#include <psapi.h>
class NetworkStatistics : public RefSingleton<NetworkStatistics>
{
public:
	PDH_HQUERY _cpuQuery = {};
	PDH_HCOUNTER _cpuTotal = {};
	PDH_FMT_COUNTERVALUE _counterVal = {};
	std::atomic_int64_t mBufferUsedCount = 0;
	std::atomic_int64_t mJobCount = 0;

	std::atomic_int64_t mDBJobCount = 0;
	std::atomic_int64_t mTotalDBJobCount = 0;

	std::atomic_bool mIsRunning = false;
public:

	NetworkStatistics();
	~NetworkStatistics() = default;

	void PrintStatistics();
	void SetIsRunning(bool flag) { mIsRunning = flag; }

	// CPU 사용량
	const double GetCPUUsage();
	// 메모리 사용량
	const uint64_t GetMemoryUsage_Byte();
	const double GetMemoryUsage_KB();
	const double GetMemoryUsage_MB();

};

