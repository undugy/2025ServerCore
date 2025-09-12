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
		std::vector<T*> temp;
		temp.reserve(size);
		{
			std::lock_guard<std::recursive_mutex> guard(mLock);
			for (size_t i = 0; i < size; i++)
			{
				T* raw = static_cast<T*>(mi_malloc(sizeof(T)));
				if (!raw) throw std::bad_alloc();
				new (raw) T(std::forward<Args>(args)...);
				temp.push_back(raw);
			}
		}
		for(T* ptr : temp)
			Release(ptr);

		mAllocCount += size;
	}
	template <typename... Args>
	std::shared_ptr<T> Acquire(Args&&... args)
	{
		T* row = New(std::forward<Args>(args)...);
		std::shared_ptr<T> ptr(
			row,
			[this](T* obj) { this->Release(obj); }
		);
		return ptr;
	}

	template <typename... Args>
	T* New(Args&&... args)
	{
		T* ptr = nullptr;
		std::lock_guard<std::recursive_mutex> guard(mLock);
		if (mPool.empty())
		{
			ptr = make_mimalloc(std::forward<Args>(args)...);
			mAllocCount++;
		}
		else
		{
			ptr = mPool.top();
			new(ptr)T(std::forward<Args>(args)...);
			mPool.pop();
		}
		
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

