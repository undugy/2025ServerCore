#pragma once
#include <stack>
#include <mutex>
#include <cstdint>
#include <memory>
#include <functional>
#include"RefSingleton.h"
template<typename T>
class ObjectPool:public RefSingleton<ObjectPool<T>>
{
	std::stack<T*> mPool;
	std::recursive_mutex mLock;
	size_t mAllocCount = 0;
public:
	ObjectPool() = default;
	~ObjectPool()
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);
		while (!mPool.empty())
		{
			void* ptr = mPool.top();
			if (ptr != nullptr)
				mi_free(ptr);
			mPool.pop();
		}
	}

	template <typename... Args>
	T* make_mimalloc(Args&&... args) {
		T* raw = static_cast<T*>(mi_malloc(sizeof(T)));
		if (!raw) throw std::bad_alloc();
		new (raw) T(std::forward<Args>(args)...);
		return raw;
	}

	size_t GetUsingCount()
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);
		return mAllocCount - mPool.size();
	}

	size_t GetAllocCount()
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);
		return mAllocCount;
	}
	template <typename... Args>
	void Reserve(size_t size, Args&&... args)
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);

		for (size_t i = 0; i < size; i++)
		{
			T* raw = static_cast<T*>(mi_malloc(sizeof(T)));
			if (!raw) throw std::bad_alloc();
			new (raw) T(std::forward<Args>(args)...);
			mPool.push(raw);
		}

		mAllocCount += size;
	}
	template <typename... Args>
	std::shared_ptr<T> Acquire(Args&&... args)
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);

		if (mPool.empty())
		{
			mPool.push(make_mimalloc(std::forward<Args>(args)...));
			mAllocCount++;
			std::shared_ptr<T> ptr(
				mPool.top(),
				[this](T* obj) { this->Release(obj); }
			);
			mPool.pop();
			return ptr;
		}

		T* row = mPool.top();
		new(row)T(std::forward<Args>(args)...);
		std::shared_ptr<T> ptr(
			row,
			[this](T* obj) { this->Release(obj); }
		);

		mPool.pop();
		return ptr;
	}

	template <typename... Args>
	T* New(Args&&... args)
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);

		if (mPool.empty())
		{
			mPool.push(make_mimalloc(std::forward<Args>(args)...));
			mAllocCount++;
		}

		T* ptr = mPool.top();
		mPool.pop();
		return ptr;
	}

	void Delete(T* obj)
	{
		Release(obj);
	}
private:
	void Release(T* obj)
	{
		std::lock_guard<std::recursive_mutex> guard(mLock);
		if (obj != nullptr)
		{
			obj->~T();
			mPool.push(std::move(obj));
		}
	}
};

