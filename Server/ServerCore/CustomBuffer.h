#pragma once
class CustomBuffer
{
public:
	CustomBuffer(const int32_t& capacity);
	CustomBuffer() = default;
	~CustomBuffer();

	void Init(const int32_t& capacity);
	void Clean();
	bool OnRead(int32_t numOfBytes);
	bool OnWrite(int32_t numOfBytes);

	BYTE* ReadPos() { return &mBuffer[mReadPos]; }
	BYTE* WritePos() { return &mBuffer[mWritePos]; }
	int32_t DataSize() { return mWritePos - mReadPos; }
	int32_t FreeSize() { return mCapacity - mWritePos; }
	int32_t WriteOffset() { return mWritePos; }

private:
	int32_t mCapacity = 0;
	int32_t mReSizePos = 0;
	int32_t mReadPos = 0;
	int32_t mWritePos = 0;
	std::vector<BYTE> mBuffer;
};

