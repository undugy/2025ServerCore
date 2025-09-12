#include "pch.h"
#include "ListenerService.h"
#include "Listener.h"
#include "ObjectPool.h"
#include "IocpHandler.h"
#include "IocpContext.h"
ListenerService::ListenerService(SocketAddress targetAddress, std::shared_ptr<IocpHandler> iocpHandler, std::function<std::shared_ptr<class NetworkSession>(void)> consturctor, int32_t maxSessionCount)
	: Service(ServiceType::Connector, targetAddress, iocpHandler, consturctor, maxSessionCount)
{
	
}

bool ListenerService::Start()
{
	if (CanStart() == false)
		return false;

	mListener = std::make_shared<Listener>();

	std::shared_ptr<ListenerService> service = static_pointer_cast<ListenerService>(shared_from_this());
	if (mListener->StartAccept(service) == false)
		return false;

	return true;
}

void ListenerService::CloseService()
{
	Service::CloseService();
}

bool ListenerService::CreateRIOCQ()
{
	constexpr int PendingCount = 32;	// (Send + Recv) OutStandingCount
	constexpr int MaxClientSize = 5000 * 2;
	constexpr int CompletionQueueSize = (PendingCount * MaxClientSize);

	std::shared_ptr<RIONotifyEvent> event = ObjectPool<RIONotifyEvent>::GetInstance().Acquire(GetHandle());
	event->rioCQ = SocketUtil::RIOEFTable.RIOCreateCompletionQueue(CompletionQueueSize, &event->notify);
	if (event->rioCQ == RIO_INVALID_CQ)
	{
		int32_t errCode = ::GetLastError();
		VIEW_WRITE_ERROR("CompletionQueue Create Fail Err : {}", errCode);
		return false;
	}
	if (SocketUtil::RIOEFTable.RIONotify(event->rioCQ) != ERROR_SUCCESS)
	{
		int32_t errCode = ::GetLastError();
		VIEW_WRITE_ERROR("CompletionQueue Create Fail Err : {}", errCode);
		return false;
	}

	event->mDispatcher = mListener;
	mRioCQEventList.push_back(event);
	return true;
}
