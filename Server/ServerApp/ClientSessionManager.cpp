#include "ServerPch.h"
#include "ClientSessionManager.h"
#include "StrUtil.h"
#include "ClientSession.h"
void ClientSessionManager::OnConnected(std::shared_ptr<PacketSession> session)
{
	if (session == nullptr)
		return;
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return;
	std::lock_guard<std::recursive_mutex> guard(mSessionLock);
	_sessions.insert(cs);
}

void ClientSessionManager::OnDisconnected(std::shared_ptr<PacketSession> session)
{
	if (session == nullptr)
		return;
	std::shared_ptr<ClientSession> cs = static_pointer_cast<ClientSession>(session);
	if (cs == nullptr)
		return;
	std::lock_guard<std::recursive_mutex> guard(mSessionLock);
	_sessions.erase(cs);
}
