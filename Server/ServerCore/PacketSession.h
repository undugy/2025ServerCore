#pragma once
#include "NetworkSession.h"
struct PacketHeader
{
	uint32_t size;
	uint32_t id;
};
class PacketSession :
	public NetworkSession
{
public:
	PacketSession();
	virtual ~PacketSession();

	std::shared_ptr<PacketSession> GetPacketSession() { return std::static_pointer_cast<PacketSession>(shared_from_this()); }

protected:
	virtual int32_t OnRecv(BYTE* buffer, int32_t len) final;
	virtual void OnRecvPacket(BYTE* buffer, int32_t len) {};


};

