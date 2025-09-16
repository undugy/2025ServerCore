#pragma once
#include "PacketSession.h"
class ClientSession :
    public PacketSession
{
	int mRoomID = -1;
public:
	ClientSession() {}
	virtual ~ClientSession() {}

	void OnConnected(const SocketAddress& netAddr) override;
	void OnDisconnected() override;
	void OnRecvPacket(BYTE* buffer, int32_t len) override;
	void OnSend(int32_t len) override;

public:
	void Send(uint32_t protocol, google::protobuf::Message& pkt);
	void SetRoomID(int32_t roomID) { mRoomID = roomID; }
	const int32_t& GetRoomID() const { return mRoomID; }
};

