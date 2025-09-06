#pragma once
constexpr int32_t PAGE_SIZE = 4096;
constexpr int32_t RECV_BUFFER_SIZE = PAGE_SIZE * 4;
constexpr int32_t SEND_BUFFER_SIZE = RECV_BUFFER_SIZE * 2;

class NetworkBuffer
{
public:
	NetworkBuffer(const RIO_BUFFERID& rioBuffID ,BYTE* pBuffer, const int32_t& capacity);
	~NetworkBuffer();

	void Reset();
	void Clean();
	void Release();
	bool OnRead(int32_t numOfBytes);
	bool OnWrite(int32_t numOfBytes);

	BYTE* ReadPos() { return &mBuffer[mReadPos]; }
	BYTE* WritePos() { return &mBuffer[mWritePos]; }
	int32_t DataSize() { return mWritePos - mReadPos; }
	int32_t FreeSize() { return mCapacity - mWritePos; }
	int32_t WriteOffset() { return mWritePos; }

	RIO_BUFFERID GetRioBufferID() { return mRioBuffID; }
	
private:
	int32_t mCapacity = 0;
	int32_t mReSizePos = 0;
	int32_t mReadPos = 0;
	int32_t mWritePos = 0;
	RIO_BUFFERID mRioBuffID = 0;
	BYTE* mBuffer = nullptr;
};

