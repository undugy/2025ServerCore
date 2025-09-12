#pragma once
#include "ClientSession.h"
#include "RefSingleton.h"
#include "ClientPacketHandler.h"

class ClientSessionManager : public RefSingleton<ClientSessionManager>
{
public:
	void OnConnected(std::shared_ptr<PacketSession> session);
	void OnDisconnected(std::shared_ptr<PacketSession> session);

	template<class T>
	void Broadcast(uint32_t protocol, T& pkt)
	{
		std::lock_guard<std::recursive_mutex> guard(mSessionLock);
		for (auto& cs : _sessions)
			cs->Send(protocol, pkt);
	}

	int32_t GetSessionCount()
	{
		std::lock_guard<std::recursive_mutex> guard(mSessionLock);
		return static_cast<int32_t>(_sessions.size());
	}
private:
	std::recursive_mutex mSessionLock;
	std::unordered_set<std::shared_ptr<ClientSession>> _sessions;
};