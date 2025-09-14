#include "ServerPch.h"
#include "ChatRoomManager.h"

ChatRoomManager::ChatRoomManager()
{
	mChatRoomList.resize(50);
	for (int32_t i = 0; i < 50; i++)
		mChatRoomList[i] = ObjectPool<ChatRoom>::GetInstance().Acquire(++mNextRoomID);
}

std::shared_ptr<ChatRoom> ChatRoomManager::GetChatRoom(int32_t workId)
{
	return mChatRoomList[workId % 50];
}
