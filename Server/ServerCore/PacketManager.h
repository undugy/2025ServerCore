#pragma once
#include "RefSingleton.h"
class Packet;
class PacketChunk;
constexpr int32_t TRY_POP_COUNT = 10;
class PacketManager : public RefSingleton<PacketManager>
{
public:
	std::shared_ptr<Packet> Open(uint32_t size);

private:
	std::shared_ptr<PacketChunk> Pop();
	void Push(std::shared_ptr<PacketChunk> buffer);

	static void PushGlobal(PacketChunk* buffer);

private:
	moodycamel::ConcurrentQueue<std::shared_ptr<PacketChunk>> mPacketChunks;
};

