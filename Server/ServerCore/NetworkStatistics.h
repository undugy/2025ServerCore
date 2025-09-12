#pragma once
class NetworkStatistics : public RefSingleton<NetworkStatistics>
{
public:

	std::atomic_int64_t mBufferUsedCount = 0;
	std::atomic_int64_t mJobCount = 0;

	std::atomic_int64_t mDBJobCount = 0;
	std::atomic_int64_t mTotalDBJobCount = 0;

	std::atomic_bool mIsRunning = false;
public:
	NetworkStatistics() = default;
	~NetworkStatistics() = default;

	void PrintStatistics();
	void SetIsRunning(bool flag) { mIsRunning = flag; }
};

