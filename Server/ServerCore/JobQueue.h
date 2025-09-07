#pragma once
#include "Job.h"
#include "JobTimer.h"
#include "ObjectPool.h"
constexpr int32_t BULK_DEQUEUE_JOB_COUNT = 1000;
class JobQueue : public std::enable_shared_from_this<JobQueue>
{
public:
	void DoAsync(CallbackType&& callback)
	{
		Push(ObjectPool<Job>::GetInstance().Acquire(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		Push(ObjectPool<Job>::GetInstance().Acquire(owner, memFunc, std::forward<Args>(args)...));
	}

	std::shared_ptr<Job> MakeJob(CallbackType&& callback)
	{
		return ObjectPool<Job>::GetInstance().Acquire(std::move(callback));
	}

	template<typename T, typename Ret, typename... Args>
	std::shared_ptr<Job> MakeJob(Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		return ObjectPool<Job>::GetInstance().Acquire(owner, memFunc, std::forward<Args>(args)...);
	}

	void DoTimer(uint64_t tickAfter, CallbackType&& callback)
	{
		std::shared_ptr<Job> job = ObjectPool<Job>::GetInstance().Acquire(std::move(callback));
		JobTimer::GetInstance().Reserve(tickAfter, shared_from_this(), job);
	}

	template<typename T, typename Ret, typename... Args>
	void DoTimer(uint64_t tickAfter, Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		std::shared_ptr<Job> job = ObjectPool<Job>::GetInstance().Acquire(owner, memFunc, std::forward<Args>(args)...);
		JobTimer::GetInstance().Reserve(tickAfter, shared_from_this(), job);
	}


public:
	void Push(std::shared_ptr<Job> job, bool pushOnly = false);
	void Execute();

protected:
	moodycamel::ConcurrentQueue<std::shared_ptr<Job>> mJobQueue;
	std::atomic_int32_t					mJobCount = 0;
};
