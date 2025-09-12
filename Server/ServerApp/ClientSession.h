#pragma once
#include "PacketSession.h"
class ClientSession :
    public PacketSession
{
public:
	ClientSession() {}
	virtual ~ClientSession() {}

	void OnConnected(const SocketAddress& netAddr) override;
	void OnDisconnected() override;
	void OnRecvPacket(BYTE* buffer, int32_t len) override;
	void OnSend(int32_t len) override;

public:
	void Send(uint32_t protocol, google::protobuf::Message& pkt);
};

