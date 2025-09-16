#include "ServerPch.h"
#include "ChatRoomManager.h"

ChatRoomManager::ChatRoomManager()
{
	mChatRoomList.resize(MAX_CHATROOM_COUNT);
	for (int32_t i = 0; i < MAX_CHATROOM_COUNT; i++)
		mChatRoomList[i] = ObjectPool<ChatRoom>::GetInstance().Acquire(mNextRoomID++);
}

std::shared_ptr<ChatRoom> ChatRoomManager::GetChatRoom(int32_t workId)
{
	return mChatRoomList[workId % MAX_CHATROOM_COUNT];
}
