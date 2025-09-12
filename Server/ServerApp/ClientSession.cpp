#include "ServerPch.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"
#include "ClientSessionManager.h"
void ClientSession::OnConnected(const SocketAddress& netAddr)
{
	ClientSessionManager::GetInstance().OnConnected(GetPacketSession());
}

void ClientSession::OnDisconnected()
{
	ClientSessionManager::GetInstance().OnDisconnected(GetPacketSession());
}

void ClientSession::OnRecvPacket(BYTE* buffer, int32_t len)
{
	std::shared_ptr<PacketSession> session = GetPacketSession();
	PacketHeader header = *reinterpret_cast<PacketHeader*>(buffer);
	if (ClientPacketHandler::GetInstance().HandlePacket(session, buffer, len) == false)
	{
		VIEW_ERROR("Invalid Packet ID: {0}, Size: {1}", header.id, header.size);
	}
}

void ClientSession::OnSend([[maybe_unused]]int32_t len)
{

}

void ClientSession::Send(uint32_t protocol, google::protobuf::Message& pkt)
{
	PacketSession::Send(ClientPacketHandler::GetInstance().MakePacketBuffer(pkt, protocol));
}
