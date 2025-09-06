#include "pch.h"
#include "Packet.h"
#include "PacketChunk.h"
Packet::Packet(std::shared_ptr<PacketChunk> owner, BYTE* buffer, uint32_t allocSize)
	: mBuffer(buffer), mOwnerChunk(owner), mAllocSize(allocSize)
{

}

Packet::~Packet()
{

}

void Packet::Close(uint32_t writeSize)
{
	if (mAllocSize < writeSize)
	{
		VIEW_ERROR("Packet Close Fail!! AllocSize:%d, WriteSize:%d", mAllocSize, writeSize);
		return;
	}
	mWriteSize = writeSize;
	mOwnerChunk->Close(mWriteSize);

}

