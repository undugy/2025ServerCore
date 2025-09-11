#pragma once
#include "SocketUtil.h"
#include <functional>
#include <unordered_set>

enum class ServiceType : uint8_t
{
	Listener,
	Connector
};

//Session하나에49152/ 동접 5000이면  245, 760, 000
class IocpHandler;
class RIONotifyEvent;
class NetworkSession;
class Service : public std::enable_shared_from_this<Service>
{
protected:
	ServiceType mServiceType;
	SocketAddress mSockAddress = {};
	std::shared_ptr<IocpHandler> mIocpHandler;

	std::unordered_set<std::shared_ptr<NetworkSession>> mSessions;
	int32_t mSessionCount = 0;
	int32_t mMaxSessionCount = 0;
	std::function<std::shared_ptr<class NetworkSession>(void)>mSessionConstructor;

	std::vector<std::shared_ptr<RIONotifyEvent>> mRioCQEventList;

	std::recursive_mutex mSessionLock;
public:
	Service(ServiceType type, SocketAddress address, std::shared_ptr<IocpHandler> iocpHandler, std::function<std::shared_ptr<class NetworkSession>(void)> consturctor, int32_t maxSessionCount = 1);
	virtual ~Service();
	virtual bool Start() = 0;
	bool CanStart() const { return mSessionConstructor != nullptr; }
	virtual void CloseService() {};

	std::shared_ptr<NetworkSession> CreateSession();
	void AddSession(std::shared_ptr<NetworkSession> session);
	void ReleaseSession(std::shared_ptr<NetworkSession> session);
	int32_t GetCurrentSessionCount() { return mSessionCount; }
	int32_t GetMaxSessionCount() { return mMaxSessionCount; }

	ServiceType GetServiceType() { return mServiceType; }
	SocketAddress GetSockAddress() { return mSockAddress; }
	RIO_CQ& GetRIOCQ(int32_t index);

	std::shared_ptr<IocpHandler> GetIocpHandler() { return mIocpHandler; }
	HANDLE GetHandle(); 
};