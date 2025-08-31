#pragma once
#include<atomic>
#include"SocketUtil.h"
#include"RefSingleton.h"
constexpr bool USE_LARGE_PAGES = false; 
class NetworkBuffer;
template <size_t PS, size_t S>
class RegisteredBufferPool :public RefSingleton<RegisteredBufferPool>
{
	moodycamel::ConcurrentQueue<NetworkBuffer*> mFreeQueue;
	BYTE* mBuffer = nullptr;
	std::recursive_mutex mLock;
	std::atomic_int mAllocCount = 0;
	std::atomic_int mUseCount = 0;
public:
	RegisteredBufferPool() = default;
	~RegisteredBufferPool()
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
	
	void Initialize() override
	{
		DWORD totalBuffersAllocated = 0;
		while (totalBuffersAllocated < PS)
		{
			DWORD bufferSize = 0;
			DWORD receiveBuffersAllocated = 0;

			BYTE* pBuffer = AllocateBufferSpace(S, PS, bufferSize, receiveBuffersAllocated);

			totalBuffersAllocated += receiveBuffersAllocated;


			RIO_BUFFERID id = SocketUtil::RIOEFTable.RIORegisterBuffer(pBuffer, static_cast<DWORD>(bufferSize));
			if (id == RIO_INVALID_BUFFERID)
			{
				//·Î±ë
			}

			mFreeQueue.enqueue(AllocBuffer(id, pBuffer, S));
			mAllocCount.fetch_add(1);
			//if (totalBuffersAllocated != pendingRecvs)
			//{
			//	//·Î±ë
			//}
		}


	}

	size_t GetUsingCount()
	{
		return mUseCount;
	}

	size_t GetFreeCount()
	{
		return mAllocCount - mUseCount;
	}

	template <typename... Args>
	void Reserve(size_t size, Args... args)
	{
		DWORD totalBuffersAllocated = 0;
		while (totalBuffersAllocated < size)
		{
			DWORD bufferSize = 0;
			DWORD receiveBuffersAllocated = 0;

			BYTE* pBuffer = AllocateBufferSpace(S, size, bufferSize, receiveBuffersAllocated);

			totalBuffersAllocated += receiveBuffersAllocated;


			RIO_BUFFERID id = SocketUtil::RIOEFTable.RIORegisterBuffer(pBuffer, static_cast<DWORD>(bufferSize));
			if (id == RIO_INVALID_BUFFERID)
			{
				//·Î±ë
			}

			mFreeQueue.enqueue(AllocBuffer(id, pBuffer, S));
			mAllocCount.fetch_add(1);

		}
	}
	template <typename... Args>
	std::shared_ptr<T> Acquire(Args... args)
	{
		NetworkBuffer* pNetBuff = nullptr;
		if (GetFreeCount() <= 0)
		{
			DWORD bufferSize = 0;
			DWORD receiveBuffersAllocated = 0;
			BYTE* pBuffer = AllocateBufferSpace(S, 1, bufferSize, receiveBuffersAllocated);
			RIO_BUFFERID id = SocketUtil::RIOEFTable.RIORegisterBuffer(pBuffer, static_cast<DWORD>(bufferSize));
			if (id == RIO_INVALID_BUFFERID)
			{
				//·Î±ë
				return nullptr
			}
			pNetBuff = AllocBuffer(id, pBuffer, S));
			mAllocCount.fetch_add(1);
			mUseCount.fetch_add(1);

		}
		else
		{
			while (false == queue.try_dequeue(pNetBuff)) {};
			mUseCount.fetch_add(1);
		}

		if(pNetBuff == nullptr)
			return nullptr;

		std::shared_ptr<NetworkBuffer> ptr(
			pNetBuff,
			[this](NetworkBuffer* obj) { this->Release(obj); }
		);

		return ptr;
	}

	inline BYTE* AllocateBufferSpace(
		const DWORD pendigBufferSize,
		const DWORD pendingRecvs,
		DWORD& bufferSize,
		DWORD& receiveBuffersAllocated)
	{
		const DWORD preferredNumaNode = 0;

		const SIZE_T largePageMinimum = USE_LARGE_PAGES ? ::GetLargePageMinimum() : 0;

		SYSTEM_INFO systemInfo;

		::GetSystemInfo(&systemInfo);

		systemInfo.dwAllocationGranularity;

		const unsigned __int64 granularity = (largePageMinimum == 0 ? systemInfo.dwAllocationGranularity : largePageMinimum);

		const unsigned __int64 desiredSize = pendigBufferSize * pendingRecvs;

		unsigned __int64 actualSize = RoundUp(desiredSize, granularity);

		if (actualSize > std::numeric_limits<DWORD>::max())
		{
			actualSize = (std::numeric_limits<DWORD>::max() / granularity) * granularity;
		}

		receiveBuffersAllocated = std::min<DWORD>(pendingRecvs, static_cast<DWORD>(actualSize / pendigBufferSize));

		bufferSize = static_cast<DWORD>(actualSize);

		BYTE* pBuffer = reinterpret_cast<BYTE*>(VirtualAllocExNuma(GetCurrentProcess(), 0, bufferSize, MEM_COMMIT | MEM_RESERVE | (largePageMinimum != 0 ? MEM_LARGE_PAGES : 0), PAGE_READWRITE, preferredNumaNode));


		return pBuffer;
	}

	inline BYTE* AllocateBufferSpace(
		const DWORD recvBufferSize,
		const DWORD pendingRecvs,
		DWORD& receiveBuffersAllocated)
	{
		DWORD notUsed;

		return AllocateBufferSpace(recvBufferSize, pendingRecvs, notUsed, receiveBuffersAllocated);
	}



private:
	void Release(T* obj)
	{
		if (obj != nullptr)
		{
			obj->~T();
			mFreeQueue.enqueue(obj);
			mUseCount.fetch_sub(1);
		}
	}

	template <typename... Args>
	NetworkBuffer * AllocBuffer(Args... args) {
		NetworkBuffer* raw = static_cast<NetworkBuffer*>(mi_malloc(sizeof(T)));
		if (!raw) throw std::bad_alloc();
		new (raw) T(std::forward<Args>(args)...);
		return raw;
	}
};

template <typename TV, typename TM>
inline TV RoundDown(TV Value, TM Multiple)
{
	return((Value / Multiple) * Multiple);
}

template <typename TV, typename TM>
inline TV RoundUp(TV Value, TM Multiple)
{
	return(RoundDown(Value, Multiple) + (((Value % Multiple) > 0) ? Multiple : 0));
}