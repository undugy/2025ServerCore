#pragma once
#include "JobQueue.h"
#include <cstdint>
class ClientSession;
class spChatRequest;
class ChatRoom : public JobQueue
{
public:
	ChatRoom();
	ChatRoom(const int32_t roomID);
	virtual ~ChatRoom();
	void Enter(std::shared_ptr<ClientSession> session);
	void Leave(std::shared_ptr<ClientSession> session);
	void Broadcast(uint32_t protocol, google::protobuf::Message& pkt);
public:
	void OnSCChatResponse(std::shared_ptr<spChatRequest> res);
private:
	int32_t mRoomID = 0;
	std::unordered_map<int, std::shared_ptr<ClientSession>> mSessionList;
};

