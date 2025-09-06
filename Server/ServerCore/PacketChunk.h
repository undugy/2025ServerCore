#pragma once
constexpr int32_t PACKET_CHUNK_SIZE = 12000;
class Packet;
class PacketChunk : public std::enable_shared_from_this<PacketChunk>
{
public:
	PacketChunk() {};
	~PacketChunk() {};

	void Reset();
	std::shared_ptr<Packet> Open(uint32_t allocSize);
	void Close(uint32_t writeSize);

	bool IsOpen() { return mIsOpen; }
	BYTE* Buffer() { return &mBuffers[mUsedSize]; }
	uint32_t FreeSize() { return static_cast<uint32_t>(mBuffers.size()) - mUsedSize; }

private:
	std::array<BYTE, PACKET_CHUNK_SIZE> mBuffers = {};
	bool mIsOpen = false;
	uint32_t mUsedSize = 0;
};

