#pragma once
#include"ChatRoom.h"
#include"RefSingleton.h"
class ChatRoomManager : public RefSingleton<ChatRoomManager>
{
public:
	ChatRoomManager();

	std::shared_ptr<ChatRoom> GetChatRoom(int32_t workId);

private:
	int32_t mNextRoomID = 0;
	std::vector<std::shared_ptr<ChatRoom>> mChatRoomList;
};

