#include "pch.h"
#include "ObjectPool.h"
#include "Packet.h"
#include "PacketChunk.h"
#include "PacketManager.h"
void PacketChunk::Reset()
{
	mIsOpen = false;
	mUsedSize = 0;
}

std::shared_ptr<Packet> PacketChunk::Open(uint32_t allocSize)
{
	if (allocSize > PACKET_CHUNK_SIZE)
	{
		VIEW_ERROR("PacketChunk Open Fail!! AllocSize:%d", allocSize);
		return nullptr;
	}
	
	if(mIsOpen == true)
	{
		VIEW_ERROR("PacketChunk Open Fail!! IsOpen True");
		return nullptr;
	}

	if (allocSize > FreeSize())
		return nullptr;
	mIsOpen = true;
	
	return ObjectPool<Packet>::GetInstance().Acquire(shared_from_this(), Buffer(), allocSize);

}


void PacketChunk::Close(uint32_t writeSize)
{
	if (mIsOpen == false)
	{
		VIEW_ERROR("PacketChunk Close Fail!! Aleady Closed");
		return ;
	}
	mIsOpen = false;
	mUsedSize += writeSize;
}