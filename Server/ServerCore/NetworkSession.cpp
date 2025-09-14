#include "pch.h"
#include "NetworkSession.h"
#include "SocketUtil.h"
#include "Service.h"

#include "RegisteredBufferPool.h"
#include "Packet.h"
NetworkSession::NetworkSession()
{
	mSocket = SocketUtil::CreateSocket();
}

NetworkSession::~NetworkSession()
{
	SocketUtil::Close(mSocket);
	mRecvBuffer = nullptr;
	mSendBuffer = nullptr;
	mPacketCount = 0;
}

void NetworkSession::Send(std::shared_ptr<Packet> buffer)
{
	if(IsConnected() == false)
		return;
	bool registerSend = false;

	{
		mSendQueue.enqueue(buffer);
		mPacketCount++;
		if (mSendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

bool NetworkSession::Connect()
{
	return RegisterConnect();
}

void NetworkSession::Disconnect(const WCHAR* cause)
{
	if (mIsConnected.exchange(false) == false)
		return;

	
	RegisterDisconnect();
}

std::shared_ptr<Service> NetworkSession::GetService()
{
	return mService.lock();
}

void NetworkSession::SetService(std::shared_ptr<Service> service)
{
	mService = service;
}

void NetworkSession::SetNetAddr(const SocketAddress& address)
{
	mSockAddr = address;
}

const SocketAddress& NetworkSession::GetAddress() const
{
	return mSockAddr;
}

bool NetworkSession::RegisterRIOBuffer()
{
	mRecvBuffer = RegisteredBufferPool<RECV_BUFFER_SIZE>::GetInstance().Acquire();
	if (mRecvBuffer == nullptr)
		return false;
	mSendBuffer = RegisteredBufferPool<SEND_BUFFER_SIZE>::GetInstance().Acquire();
	if (mSendBuffer == nullptr)
		return false;
	return true;
}

bool NetworkSession::CreateRIORQ()
{
	mRioRQ = SocketUtil::RIOEFTable.RIOCreateRequestQueue(mSocket
		, OUTSTANDING_COUNT					// Max OutStanding Recv
		, 1									// Recv Buffer Count Must be 1?
		, OUTSTANDING_COUNT					// Max OutStanding Send
		, 1									// Send Buffer Count Must be 1?
		, GetService()->GetRIOCQ(GetSessionID())
		, GetService()->GetRIOCQ(GetSessionID())
		, nullptr);
	if (mRioRQ == RIO_INVALID_RQ)
	{
		int32_t errorCode = ::GetLastError();
		HandleError(errorCode);
		Disconnect(L"Request Queue Create Fail");
		return false;
	}

	return true;
}

HANDLE NetworkSession::GetHandle()
{
	return reinterpret_cast<HANDLE>(mSocket);
}

void NetworkSession::Dispatch(IocpContext* iocpContext, int32_t numOfBytes)
{
	if(iocpContext == nullptr)
		return;

	switch (iocpContext->mContextType)
	{
	case ContextType::Connect:
		ProcessConnect();
		break;
	case ContextType::Disconnect:
		ProcessDisconnect();
		break;
#ifdef VERSION_RIO
#else
	case ContextType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case ContextType::Send:
		ProcessSend(numOfBytes);
		break;
#endif
	default:
		break;
	}
}

void NetworkSession::Dispatch(RIOContext* rioContext, int32_t numOfBytes)
{
	switch (rioContext->mContextType)
	{	// RIO는 Send / Recv만 작동
	case ContextType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case ContextType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}

void NetworkSession::Dispatch(RIONotifyEvent* notifyEvent)
{
	if(notifyEvent == nullptr)
		return;

	if (notifyEvent == nullptr)
		return;

	RIORESULT results[RIO_DISPATCH_RESULT_COUNT] = { 0, };

	uint32_t numResult = 0;
	{
		std::lock_guard<std::recursive_mutex> guard(mRQLock);
		numResult = SocketUtil::RIOEFTable.RIODequeueCompletion(notifyEvent->rioCQ, results, RIO_DISPATCH_RESULT_COUNT);
		SocketUtil::RIOEFTable.RIONotify(notifyEvent->rioCQ);
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
		if (context == nullptr)
			continue;
		std::shared_ptr<IDispatcher> dispatcher = context->mDispatcher;
		if (dispatcher == nullptr)
			continue;
		dispatcher->Dispatch(context, results[i].BytesTransferred);
	}

}

void NetworkSession::SetSessionID()
{
	static std::atomic_int32_t sSessionID = 1;
	mSessionID = sSessionID.fetch_add(1);
}

bool NetworkSession::RegisterConnect()
{
	if (IsConnected())
		return false;

	if(GetService() == nullptr)
		return false;

	if (GetService()->GetServiceType() != ServiceType::Connector)
		return false;

	if (SocketUtil::SetReuseAddress(mSocket, true) == false)
		return false;

	if (SocketUtil::BindAnyAddress(mSocket, 0) == false)
		return false;

	mConnectContext.Init();
	mConnectContext.mDispatcher = shared_from_this();

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetSockAddress().GetSockAddr();
	if (SocketUtil::ConnectEx(mSocket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &mConnectContext) == false)
	{
		int32_t errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			mConnectContext.mDispatcher = nullptr;
			return false;
		}
	}

	return true;
}

bool NetworkSession::RegisterDisconnect()
{
	mDisconnectContext.Init();
	mDisconnectContext.mDispatcher = shared_from_this();

	if (false == SocketUtil::DisconnectEx(mSocket, &mDisconnectContext, TF_REUSE_SOCKET, 0))
	{
		int32_t errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			mDisconnectContext.mDispatcher = nullptr; // RELEASE_REF
			return false;
		}
	}

	return true;
}

void NetworkSession::RegisterRecv()
{
	if (IsConnected() == false)
		return;
	if(mRecvBuffer == nullptr)
		return;
#ifdef VERSION_RIO
	mRioRecvContext.Init();
	mRioRecvContext.mDispatcher = shared_from_this();

	mRioRecvContext.BufferId = mRecvBuffer->GetRioBufferID();
	mRioRecvContext.Length = mRecvBuffer->FreeSize();
	mRioRecvContext.Offset = static_cast<uint64_t>(mRecvBuffer->GetRioOffset() + mRecvBuffer->WriteOffset());

	DWORD recvbytes = 0;
	DWORD flags = 0;

	std::lock_guard<std::recursive_mutex> guard(mRQLock);
	if (SocketUtil::RIOEFTable.RIOReceive(mRioRQ, static_cast<PRIO_BUF>(&mRioRecvContext), 1, flags, &mRioRecvContext) == false)
	{
		int32_t errorCode = ::GetLastError();
		HandleError(errorCode);
		mRioRecvContext.mDispatcher = nullptr;
	}
#else
	mRecvEvent.Init();
	mRecvEvent.mDispatcher = shared_from_this();
	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(mRecvBuffer->WritePos());
	wsaBuf.len = mRecvBuffer->FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (::WSARecv(mSocket, &wsaBuf, 1, OUT & numOfBytes, OUT & flags, &mRecvEvent, nullptr) == SOCKET_ERROR)
	{
		int32_t errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			mRecvEvent.mDispatcher = nullptr;
		}
	}
#endif
}

void NetworkSession::RegisterSend()
{
	if (IsConnected() == false)
		return;

#ifdef VERSION_RIO
		mRioSendContext.Init();
		mRioSendContext.mDispatcher = shared_from_this();
		mRioSendContext.BufferId = mSendBuffer->GetRioBufferID();
		mRioSendContext.Offset = mSendBuffer->GetRioOffset() + mSendBuffer->WriteOffset();
		std::vector<std::shared_ptr<Packet>> swapList(BULK_DEQUEUE_COUNT);
		size_t n = mSendQueue.try_dequeue_bulk(swapList.begin(), BULK_DEQUEUE_COUNT);
		int packetCount = static_cast<int>(n);
		for(int i =0; i < packetCount; ++i)
		{
			if(swapList[i] == nullptr)
				continue;
			if (mSendBuffer->FreeSize() < swapList[i]->WriteSize())
			{
				VIEW_WRITE_ERROR("There's no FreeSize Session({})", mSessionID);
				Disconnect(L"SendBuffer Over");
				return;
			}
			memcpy_s(mSendBuffer->WritePos(), swapList[i]->WriteSize(), swapList[i]->Buffer(), swapList[i]->WriteSize());
			
			mRioSendContext.Length += swapList[i]->WriteSize();
			mSendBuffer->OnWrite(swapList[i]->WriteSize());
		}
		mPacketCount.fetch_sub(packetCount);

		DWORD sendbytes = 0;
		DWORD flags = 0;
		std::lock_guard<std::recursive_mutex> guard(mRQLock);
		if (SocketUtil::RIOEFTable.RIOSend(mRioRQ, &mRioSendContext, 1, flags, &mRioSendContext) == false)
		{
			int32_t errorCode = ::WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{
				HandleError(errorCode);
				mRioSendContext.mDispatcher = nullptr;
				mSendRegistered.store(false);
			}
		}
#else
		mSendContext.Init();
		mSendContext.mDispatcher = shared_from_this();
		std::vector<std::shared_ptr<Packet>> swapList(BULK_DEQUEUE_COUNT);
		size_t n = mSendQueue.try_dequeue_bulk(swapList.begin(), BULK_DEQUEUE_COUNT);
		int32_t writeSize = 0;
		for (int i = 0; i < n; ++i)
		{
			if (swapList[i] == nullptr)
				continue;
			writeSize += swapList[i]->WriteSize();
			mSendContext.mPacketList.push_back(swapList[i]);
		}


		std::vector<WSABUF> wsaBufs;
		wsaBufs.reserve(mSendContext.mPacketList.size());
		for (std::shared_ptr<Packet> packet : mSendContext.mPacketList)
		{
			WSABUF wsaBuf;
			wsaBuf.buf = reinterpret_cast<char*>(packet->Buffer());
			wsaBuf.len = static_cast<LONG>(packet->WriteSize());
			wsaBufs.push_back(wsaBuf);
		}

		DWORD numOfBytes = 0;
		if (WSASend(mSocket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr) == SOCKET_ERROR)
		{
			int32_t errorCode = ::WSAGetLastError();
			if (errorCode != WSA_IO_PENDING)
			{
				HandleError(errorCode);
				mSendContext.mDispatcher = nullptr;
				mSendContext.mPacketList.clear();
				mSendRegistered.store(false);
			}
		}
#endif
}

void NetworkSession::ProcessConnect()
{
	mIsConnected.store(true);
	const auto& pService = GetService();
	if (pService == nullptr)
	{
		VIEW_ERROR("ProcessConnect Fail!! Service is null");
		return;
	}
	pService->AddSession(GetSession());
#ifdef VERSION_RIO
		RegisterRIOBuffer();
		CreateRIORQ();
#endif
	SetSessionID();
	OnConnected(pService->GetSockAddress());
	RegisterRecv();
}

void NetworkSession::ProcessDisconnect()
{
	mDisconnectContext.mDispatcher = nullptr;

	OnDisconnected();
	const auto& pService = GetService();
	if (pService == nullptr)
	{
		VIEW_ERROR("ProcessDisconnect Fail!! Service is null");
		return;
	}
	pService->ReleaseSession(GetSession());
	
}

void NetworkSession::ProcessRecv(int32_t numOfBytes)
{
#ifdef VERSION_RIO
		mRioRecvContext.mDispatcher = nullptr;
#else
		mRecvContext.mDispatcher = nullptr;
#endif
	if (numOfBytes == 0)
	{
		Disconnect(L"0 Bytes Recv");
		return;
	}

	if(mRecvBuffer == nullptr)
	{
		Disconnect(L"mRecvBuffer is null");
		return;
	}

	if (mRecvBuffer->OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	int32_t dataSize = mRecvBuffer->DataSize();
	int32_t processLen = OnRecv(mRecvBuffer->ReadPos(), dataSize);
	if (processLen < 0 || dataSize < processLen || mRecvBuffer->OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}

	mRecvBuffer->Clean();
	RegisterRecv();
}

void NetworkSession::ProcessSend(int32_t numOfBytes)
{
#ifdef VERSION_RIO
		mSendBuffer->OnRead(numOfBytes);
		mSendBuffer->Clean();
		mRioSendContext.mDispatcher = nullptr;
#else
		mSendContext.mDispatcher = nullptr;
		mSendContext.mPacketList.clear();
#endif
	if (numOfBytes == 0)
	{
		Disconnect(L"0 Bytes Send");
		return;
	}

	OnSend(numOfBytes);

	if (mPacketCount >= WARN_SENDQUEUE_SIZE)
		VIEW_WRITE_WARNING("SessionID({}) SendQueueSize Over {}!! QueueSize({})", GetSessionID(), WARN_SENDQUEUE_SIZE, mPacketCount.load());
	if (mPacketCount >= WARN_SENDQUEUE_SIZE)
	{
		VIEW_WRITE_WARNING("SessionID({}) Kick By SendQueueSize Over {}!! QueueSize({})", GetSessionID(), KICK_SENDQUEUE_SIZE, mPacketCount.load());
		Disconnect(L"SendQueue Size Over");
		return;
	}
	if (mPacketCount<=0)
		mSendRegistered.store(false);
	else
		RegisterSend();
}

void NetworkSession::HandleError(int32_t errorCode)
{
	errorCode;
}

const SOCKET& NetworkSession::GetSocket() const
{
	return mSocket;
}

bool NetworkSession::IsConnected()
{
	return mIsConnected;
}

int32_t NetworkSession::GetSessionID()
{
	return mSessionID;
}

std::shared_ptr<NetworkSession> NetworkSession::GetSession()
{
	return std::static_pointer_cast<NetworkSession>(shared_from_this());
}
