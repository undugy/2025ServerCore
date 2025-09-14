#include "ServerPch.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"
#include "ClientSessionManager.h"
#include "ChatRoomManager.h"
void ClientSession::OnConnected(const SocketAddress& netAddr)
{
	ClientSessionManager::GetInstance().OnConnected(GetPacketSession());
	ChatRoomManager::GetInstance().GetChatRoom(GetSessionID())->DoAsync(&ChatRoom::Enter, std::static_pointer_cast<ClientSession>(shared_from_this()));
}

void ClientSession::OnDisconnected()
{
	ClientSessionManager::GetInstance().OnDisconnected(GetPacketSession());
	ChatRoomManager::GetInstance().GetChatRoom(GetSessionID())->DoAsync(&ChatRoom::Leave, std::static_pointer_cast<ClientSession>(shared_from_this()));
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
