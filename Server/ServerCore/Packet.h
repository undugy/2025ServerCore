#pragma once
class PacketChunk;
class Packet : std::enable_shared_from_this<Packet>
{
public:
	Packet(std::shared_ptr<PacketChunk> owner, BYTE* buffer, uint32_t allocSize);
	~Packet();

	BYTE* Buffer() { return mBuffer; }
	uint32_t AllocSize() { return mAllocSize; }
	uint32_t WriteSize() { return mWriteSize; }
	void Close(uint32_t writeSize);

private:
	BYTE* mBuffer;
	uint32_t mAllocSize = 0;
	uint32_t mWriteSize = 0;
	std::shared_ptr<PacketChunk> mOwnerChunk;
};