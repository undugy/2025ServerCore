#include "ServerPch.h"
#include "ChatRoom.h"

ChatRoom::ChatRoom()
{
}

ChatRoom::ChatRoom(const int32_t roomID)
	:mRoomID(roomID)
{
}

ChatRoom::~ChatRoom()
{
}

void ChatRoom::Enter(std::shared_ptr<ClientSession> session)
{
	mSessionList.insert_or_assign(session->GetSessionID(), session);
}

void ChatRoom::Leave(std::shared_ptr<ClientSession> session)
{
	mSessionList.erase(session->GetSessionID());
}

void ChatRoom::Broadcast(uint32_t protocol, google::protobuf::Message& pkt)
{
}

void ChatRoom::OnSCChatResponse(std::shared_ptr<spChatRequest> res)
{
}
