#include "pch.h"
#include "NetworkBuffer.h"
#include "SocketUtil.h"
NetworkBuffer::NetworkBuffer(const RIO_BUFFERID& rioBuffID, BYTE* pBuffer, const int32_t& capacity)
	: mRioBuffID(rioBuffID), mBuffer(pBuffer), mCapacity(capacity), mReSizePos(capacity / 2)
{

}

NetworkBuffer::~NetworkBuffer()
{

}

void NetworkBuffer::Reset()
{
	mReadPos = 0;
	mWritePos = 0;
	ZeroMemory(mBuffer, mCapacity);
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

void NetworkBuffer::Release()
{
	mReadPos = 0;
	mWritePos = 0;
	SocketUtil::RIOEFTable.RIODeregisterBuffer(mRioBuffID);
	VirtualFreeEx(GetCurrentProcess(), mBuffer, 0, MEM_RELEASE);
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
