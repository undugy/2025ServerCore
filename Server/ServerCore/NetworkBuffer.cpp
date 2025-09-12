#include "pch.h"
#include "NetworkBuffer.h"
#include "SocketUtil.h"
#include "CustomBuffer.h"
NetworkBuffer::NetworkBuffer(const RIO_BUFFERID& rioBuffID, int32_t offset, BYTE* pBuffer, const int32_t& capacity)
	: mRioBuffID(rioBuffID),mOffset(offset), mBuffer(pBuffer + offset), mCapacity(capacity), mReSizePos(capacity / 2)
{

}

NetworkBuffer::~NetworkBuffer()
{

}

void NetworkBuffer::Reset()
{
	mReadPos = 0;
	mWritePos = 0;

}

void NetworkBuffer::Clean()
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

bool NetworkBuffer::OnRead(int32_t numOfBytes)
{
	if (numOfBytes > DataSize())
		return false;

	mReadPos += numOfBytes;
	return true;
}

bool NetworkBuffer::OnWrite(int32_t numOfBytes)
{
	if (numOfBytes > FreeSize())
		return false;

	mWritePos += numOfBytes;
	return true;
}
