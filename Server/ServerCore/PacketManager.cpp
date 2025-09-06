#include "pch.h"
#include "PacketManager.h"
#include "PacketChunk.h"
#include "Packet.h"
std::shared_ptr<Packet> PacketManager::Open(uint32_t size)
{
	if (LPacketChunk == nullptr)
	{
		LPacketChunk = Pop();
		LPacketChunk->Reset();
	}

	if (LPacketChunk->IsOpen())
	{
		VIEW_ERROR("LPacketChunk Is Aleady Open");
		return nullptr;
	}
	
	if (LPacketChunk->FreeSize() < size)
	{
		LPacketChunk = Pop();
		LPacketChunk->Reset();
	}

	return LPacketChunk->Open(size);
}
std::shared_ptr<PacketChunk> PacketManager::Pop()
{
	std::shared_ptr<PacketChunk> chunk;
	for (int i = 0; i < TRY_POP_COUNT; ++i)
	{
		if (mPacketChunks.try_dequeue(chunk))
		{
			return chunk;
		}
	}

	PacketChunk* raw = static_cast<PacketChunk*>(mi_malloc(sizeof(PacketChunk)));
	if (!raw) throw std::bad_alloc();
	return std::shared_ptr<PacketChunk>(new (raw) PacketChunk(), PushGlobal);
}

void PacketManager::Push(std::shared_ptr<PacketChunk> buffer)
{
	if (buffer == nullptr)
		return;
	mPacketChunks.enqueue(buffer);
}

void PacketManager::PushGlobal(PacketChunk* buffer)
{
	PacketManager::GetInstance().Push(std::shared_ptr<PacketChunk>(buffer, PushGlobal));
}