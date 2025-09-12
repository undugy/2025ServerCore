#pragma once
#include<atomic>
#include"SocketUtil.h"
#include"RefSingleton.h"
#include"NetworkBuffer.h"
constexpr bool USE_LARGE_PAGES = false; 

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

template<int32_t BS>
class RegisteredBufferPool :public RefSingleton<RegisteredBufferPool<BS>>
{
	moodycamel::ConcurrentQueue<NetworkBuffer*> mQueue;

	std::atomic_int mAllocCount = 0;
	std::atomic_int mUseCount = 0;
	RIO_BUFFERID mRioBuffID = RIO_INVALID_BUFFERID;
	CHAR* mBuffer = nullptr;
	uint32_t mSize = 0;//ÃÑ °³¼ö
public:
	RegisteredBufferPool() = default;
	~RegisteredBufferPool()
	{
		NetworkBuffer* ptr = nullptr;
		while (!mQueue.try_dequeue(ptr))
		{
			if (ptr != nullptr)
			{
				mi_free(ptr);
			}
		}

		SocketUtil::RIOEFTable.RIODeregisterBuffer(mRioBuffID);
		VirtualFreeEx(GetCurrentProcess(), mBuffer, 0, MEM_RELEASE);
	}
	
	void Initialize(uint32_t size)
	{
		mSize = size;
		DWORD bufferSize = 0;
		DWORD receiveBuffersAllocated = 0;
		DWORD totalBuffersAllocated = 0;
		mBuffer = AllocateBufferSpace(BS, mSize, bufferSize, receiveBuffersAllocated);
		if (bufferSize != BS * size)
		{
			VIEW_ERROR("BufferSize is Diffrent bufferSize :{}, BS: {}", bufferSize, BS * size);
		}
		mRioBuffID = SocketUtil::RIOEFTable.RIORegisterBuffer(mBuffer, static_cast<DWORD>(bufferSize));
		if (mRioBuffID == RIO_INVALID_BUFFERID)
		{
			VIEW_ERROR("RegisteredBufferPool RIORegisterBuffer Failed");
			VirtualFreeEx(GetCurrentProcess(), mBuffer, 0, MEM_RELEASE);
			return;
		}
		int32_t offset = 0;
		for (int i = 0; i < mSize; ++i)
		{
			offset = i * BS;
			mQueue.enqueue(AllocBuffer(mRioBuffID, offset, reinterpret_cast<BYTE*>(mBuffer), BS));
			mAllocCount.fetch_add(1);
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

	//template <typename... Args>
	//void Reserve(size_t size, Args... args)
	//{
	//	DWORD totalBuffersAllocated = 0;
	//	for (int i = 0; i < mSize; ++i)
	//	{
	//		DWORD bufferSize = 0;
	//		DWORD receiveBuffersAllocated = 0;

	//		BYTE* pBuffer = AllocateBufferSpace(BS, 1, bufferSize, receiveBuffersAllocated);

	//		totalBuffersAllocated += receiveBuffersAllocated;


	//		RIO_BUFFERID id = SocketUtil::RIOEFTable.RIORegisterBuffer(pBuffer, static_cast<DWORD>(bufferSize));
	//		if (id == RIO_INVALID_BUFFERID)
	//		{
	//			//·Î±ë
	//		}

	//		mQueue.enqueue(AllocBuffer(id, reinterpret_cast<BYTE*>(pBuffer), BS));
	//		mAllocCount.fetch_add(1);

	//	}
	//}
	
	std::shared_ptr<NetworkBuffer> Acquire()
	{
		NetworkBuffer* pNetBuff = nullptr;
		if (GetFreeCount() <= 0)
		{
			return nullptr;

		}
		else
		{
			while (false == mQueue.try_dequeue(pNetBuff)) {};
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

	inline CHAR* AllocateBufferSpace(
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

		CHAR* pBuffer = reinterpret_cast<CHAR*>(VirtualAllocExNuma(GetCurrentProcess(), 0, bufferSize, MEM_COMMIT | MEM_RESERVE | (largePageMinimum != 0 ? MEM_LARGE_PAGES : 0), PAGE_READWRITE, preferredNumaNode));


		return pBuffer;
	}

	inline CHAR* AllocateBufferSpace(
		const DWORD recvBufferSize,
		const DWORD pendingRecvs,
		DWORD& receiveBuffersAllocated)
	{
		DWORD notUsed;

		return AllocateBufferSpace(recvBufferSize, pendingRecvs, notUsed, receiveBuffersAllocated);
	}



private:
	void Release(NetworkBuffer* obj)
	{
		if (obj != nullptr)
		{
			obj->Reset();
			mQueue.enqueue(obj);
			mUseCount.fetch_sub(1);
		}
	}

	template <typename... Args>
	NetworkBuffer * AllocBuffer(Args... args) {
		NetworkBuffer* raw = static_cast<NetworkBuffer*>(mi_malloc(sizeof(NetworkBuffer)));
		if (!raw) throw std::bad_alloc();
		new (raw) NetworkBuffer(std::forward<Args>(args)...);
		return raw;
	}
};

