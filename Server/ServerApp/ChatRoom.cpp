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
	session->SetRoomID(mRoomID);
}

void ChatRoom::Leave(std::shared_ptr<ClientSession> session)
{
	mSessionList.erase(session->GetSessionID());
	session->SetRoomID(-1);
}

void ChatRoom::Broadcast(uint32_t protocol, google::protobuf::Message& pkt)
{
}

void ChatRoom::OnSCChatResponse(std::shared_ptr<spChatRequest> res)
{
}
