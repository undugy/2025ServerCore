#include "pch.h"
#include "Service.h"
#include "IocpContext.h"
#include "IocpHandler.h"
#include "NetworkSession.h"
Service::Service(ServiceType type, SocketAddress address, std::shared_ptr<IocpHandler> iocpHandler, std::function<std::shared_ptr<class NetworkSession>(void)> consturctor, int32_t maxSessionCount)
	: mServiceType(type), mSockAddress(address), mIocpHandler(iocpHandler), mSessionConstructor(consturctor), mMaxSessionCount(maxSessionCount)
{
}

Service::~Service()
{
#ifdef VERSION_RIO
	mRioCQEventList.clear();
#endif
}

std::shared_ptr<NetworkSession> Service::CreateSession()
{
	std::shared_ptr<NetworkSession> session = mSessionConstructor();
	session->SetService(static_pointer_cast<Service>(shared_from_this()));

	if (mIocpHandler->RegisterHandleObject(session->GetHandle()) == false)
		return nullptr;

	return session;
}

void Service::AddSession(std::shared_ptr<NetworkSession> session)
{
	std::lock_guard	<std::recursive_mutex> guard(mSessionLock);
	mSessions.insert(session);
	++mSessionCount;
}

void Service::ReleaseSession(std::shared_ptr<NetworkSession> session)
{
	std::lock_guard	<std::recursive_mutex> guard(mSessionLock);
	mSessions.erase(session);
	--mSessionCount;
}

RIO_CQ& Service::GetRIOCQ(int32_t index)
{
	return mRioCQEventList[index % mRioCQEventList.size()]->rioCQ;
}

HANDLE Service::GetHandle()
{
	if(mIocpHandler == nullptr)
		return INVALID_HANDLE_VALUE;

	return mIocpHandler->GetHandle();
}