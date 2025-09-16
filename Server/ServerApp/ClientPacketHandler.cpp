#include "ServerPch.h"
#include "ClientPacketHandler.h"
#include "GameDBData.h"
#include "ChatRoomManager.h"
void ClientPacketHandler::Init()
{
	_RegisterHandler(&ClientPacketHandler::OnCSChatRequest);
	_RegisterHandler(&ClientPacketHandler::OnCSGetIDRequest);
}

bool ClientPacketHandler::OnCSChatRequest(std::shared_ptr<PacketSession>& session, CSChatRequest& pkt)
{
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return false;

	std::shared_ptr<spChatRequest> req = ObjectPool<spChatRequest>::GetInstance().Acquire();
	req->SessionID = cs->GetSessionID();
	req->Msg = pkt.msg();
	SCChatResponse res;
	res.set_sessionid(cs->GetSessionID());
	res.set_msg("Server Recv Msg SessionID : "+std::to_string(req->SessionID));
	cs->Send(EPacketProtocol::SC_ChatResponse, res);
	//std::shared_ptr<ChatRoom> chatRoom = ChatRoomManager::GetInstance().GetChatRoom(cs->GetSessionID());
	//if (chatRoom == nullptr)
	//	return false;
	//std::shared_ptr<Job> job = chatRoom->MakeJob(&ChatRoom::OnSCChatResponse, req);
	//req->Owner = chatRoom;
	//req->ResponseJob = job;
	//GameDBLoadBalancer::GetInstance().Push(cs->GetSessionID(), EDBProtocol::SGDB_ChatRequest, req);
	return true;
}

bool ClientPacketHandler::OnCSGetIDRequest(std::shared_ptr<PacketSession>& session, CSGetIDRequest& pkt)
{
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return false;

	SCGetIDResponse res;
	res.set_sessionid(cs->GetSessionID());
	res.set_roomid(cs->GetRoomID());
	cs->Send(EPacketProtocol::SC_GetIDResponse, res);
	return true;
}
