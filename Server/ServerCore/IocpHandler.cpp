#include "pch.h"
#include "IocpHandler.h"
#include "IocpContext.h"
#include "Service.h"
IocpHandler::IocpHandler()
{
	mIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
}

IocpHandler::~IocpHandler()
{
	CloseHandle(mIocpHandle);
}

bool IocpHandler::RegisterHandleObject(HANDLE handle)
{
	return CreateIoCompletionPort(handle, mIocpHandle,0, 0);
}

bool IocpHandler::ProcessDispatch(uint32_t timeOutMs)
{
	DWORD transfferedByte = 0;
	ULONG_PTR key = 0;
	IocpContext* iocpContext = nullptr;

	if (TRUE == GetQueuedCompletionStatus(mIocpHandle,&transfferedByte, &key, reinterpret_cast<LPOVERLAPPED*>(&iocpContext), timeOutMs))
	{
		if (iocpContext == nullptr)
		{
			//·Î±ë
			return false;
		}
		std::shared_ptr<IDispatcher> dispatcher = iocpContext->mDispatcher;
		if (iocpContext->mDispatcher == nullptr)
		{
			//·Î±ë
			return false;
		}
#ifdef VERSION_RIO
		if (key == RIO_IOCP_COMPLETION)
		{
			dispatcher->Dispatch(static_cast<RIONotifyEvent*>(iocpContext));
			return true;
		}
#endif
		dispatcher->Dispatch(iocpContext, transfferedByte);
	}
	else
	{
		int32_t errorCode = WSAGetLastError();
		switch (errorCode)
		{
		case WAIT_TIMEOUT:
			return false;
		case ERROR_NETNAME_DELETED:
		default:
		{
#ifdef VERSION_RIO
			std::shared_ptr<IDispatcher> dispatcher = iocpContext->mDispatcher;
			
			if (key == RIO_IOCP_COMPLETION)
			{
				dispatcher->Dispatch(static_cast<RIONotifyEvent*>(iocpContext));
				return true;
			}
#endif
			dispatcher->Dispatch(iocpContext, transfferedByte);
		}
		break;
		}
	}

	return true;
}

