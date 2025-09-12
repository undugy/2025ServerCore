#include "pch.h"
#include "Listener.h"
#include "SocketUtil.h"	
#include "ListenerService.h"
#include "ObjectPool.h"
#include "IocpContext.h"
#include "NetworkSession.h"
#include "NetworkBuffer.h"
Listener::Listener()
{

}
Listener::~Listener()
{
	CloseSocket();
	mAcceptEvents.clear();
}

bool Listener::StartAccept(std::shared_ptr<ListenerService> service)
{
	mService = service;
	if (mService == nullptr)
		return false;

	mSocket = SocketUtil::CreateSocket();
	if (mSocket == INVALID_SOCKET)
		return false;

	if (mService->GetIocpHandler()->RegisterHandleObject(GetHandle()) == false)
		return false;

	if (SocketUtil::SetReuseAddress(mSocket, true) == false)
		return false;

	if (SocketUtil::SetLinger(mSocket, 0, 0) == false)
		return false;

	if (SocketUtil::Bind(mSocket, service->GetSockAddress()) == false)
		return false;

	if (SocketUtil::Listen(mSocket) == false)
		return false;

	const int32_t acceptCount = mService->GetMaxSessionCount();
	for (int32_t i = 0; i < acceptCount; i++)
	{
		auto acceptContext = ObjectPool<AcceptContext>::GetInstance().Acquire();
		acceptContext->mDispatcher = shared_from_this();
		mAcceptEvents.push_back(acceptContext);
		RegisterAccept(acceptContext.get());
	}

	return true;
}

void Listener::CloseSocket()
{
	SocketUtil::Close(mSocket);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(mSocket);
}

void Listener::Dispatch(IocpContext* iocpContext, int32_t numOfBytes)
{
	if (iocpContext->mContextType != ContextType::Accept)
	{
		VIEW_ERROR("Listener::Dispatch IocpContext Type Error");
		return;
	}
	auto acceptEvent = static_cast<AcceptContext*>(iocpContext);
	ProcessAccept(acceptEvent);
}

void Listener::Dispatch([[maybe_unused]] RIOContext* rioContext, [[maybe_unused]] int32_t numOfBytes)
{
	VIEW_ERROR("Listener::Dispatch RIOContext Call Error");
}

void Listener::RegisterAccept(AcceptContext* acceptContext)
{
	std::shared_ptr<NetworkSession> session = mService->CreateSession();

	acceptContext->Init();
	acceptContext->mDispatcher = session;
	session->mCustomBuffer.Init(RECV_BUFFER_SIZE);
	DWORD bytesReceived = 0;
	if (SocketUtil::AcceptEx(mSocket, session->GetSocket(), session->mCustomBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, OUT & bytesReceived, static_cast<LPOVERLAPPED>(acceptContext)) == false)
	{
		const int32_t errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			RegisterAccept(acceptContext);
		}
	}
}

void Listener::ProcessAccept(AcceptContext* acceptContext)
{
	if(acceptContext == nullptr)
		return;
	std::shared_ptr<NetworkSession> session = std::static_pointer_cast<NetworkSession>(acceptContext->mDispatcher);

	if (SocketUtil::SetUpdateAcceptSocket(session->GetSocket(), mSocket) == false)
	{
		RegisterAccept(acceptContext);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32_t sizeOfSockAddr = sizeof(sockAddress);
	const int32_t peerName = ::getpeername(session->GetSocket(), OUT reinterpret_cast<SOCKADDR*>(&sockAddress), &sizeOfSockAddr);
	if (peerName == SOCKET_ERROR)
	{
		RegisterAccept(acceptContext);
		return;
	}

	session->SetNetAddr(SocketAddress(sockAddress));
	session->ProcessConnect();
	
	RegisterAccept(acceptContext);
}
