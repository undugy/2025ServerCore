#include "pch.h"
#include "CustomBuffer.h"

CustomBuffer::CustomBuffer(const int32_t& capacity)
	:mCapacity(capacity), mReSizePos(capacity/2)
{
	mBuffer.resize(capacity);
}

CustomBuffer::~CustomBuffer()
{
}

void CustomBuffer::Init(const int32_t& capacity)
{
	mCapacity = capacity;
	mReSizePos = capacity / 2;
	mBuffer.resize(capacity);
}

void CustomBuffer::Clean()
{
	int32_t dataSize = DataSize();
	if (dataSize == 0)
	{
		mReadPos = mWritePos = 0;
	}
	else
	{
		if (FreeSize() < mReSizePos)
		{
			::memcpy(&mBuffer[0], &mBuffer[mReadPos], dataSize);
			mReadPos = 0;
			mWritePos = dataSize;
		}
	}
}

bool CustomBuffer::OnRead(int32_t numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	mReadPos += numOfBytes;
	return true;
}

bool CustomBuffer::OnWrite(int32_t numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	mWritePos += numOfBytes;
	return true;
}
