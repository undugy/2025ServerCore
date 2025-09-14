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
void Service::Dispatch(RIONotifyEvent* event)
{

	if (event == nullptr)
		return;

	RIORESULT results[RIO_DISPATCH_RESULT_COUNT] = { 0, };

	uint32_t numResult = INT32_C(0);
	{
		numResult = SocketUtil::RIOEFTable.RIODequeueCompletion(event->rioCQ, results, RIO_DISPATCH_RESULT_COUNT);
		SocketUtil::RIOEFTable.RIONotify(event->rioCQ);
	}

	if (numResult == 0 || numResult == RIO_CORRUPT_CQ)
	{
		int32_t errCode = ::GetLastError();
		VIEW_WRITE_ERROR("RIO Dispatch Error : {}", errCode);
		return;
	}

	for (uint32_t i = 0; i < numResult; i++)
	{
		RIOContext* context = reinterpret_cast<RIOContext*>(results[i].RequestContext);
		std::shared_ptr<IDispatcher> dispatcher = context->mDispatcher;
		dispatcher->Dispatch(context, results[i].BytesTransferred);

	}
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
std::shared_ptr<RIONotifyEvent> Service::GetRIONotify(int32_t index)
{
	return mRioCQEventList[index % mRioCQEventList.size()];
}

HANDLE Service::GetHandle()
{
	if(mIocpHandler == nullptr)
		return INVALID_HANDLE_VALUE;

	return mIocpHandler->GetHandle();
}