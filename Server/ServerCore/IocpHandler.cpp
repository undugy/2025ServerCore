#include "pch.h"
#include "IocpHandler.h"
#include "IocpContext.h"
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
		std::shared_ptr<IDispatcher> dispatcher = iocpContext->mDispatcher;
		dispatcher->Dispatch(iocpContext, transfferedByte);
	}
	else
	{
		int32_t errorCode = ::WSAGetLastError();
		switch (errorCode)
		{
		case WAIT_TIMEOUT:
			return false;
		default:
		{
			std::shared_ptr<IDispatcher> dispatcher = iocpContext->mDispatcher;
			dispatcher->Dispatch(iocpContext, transfferedByte);
		}
		break;
		}
	}

	return true;
}
