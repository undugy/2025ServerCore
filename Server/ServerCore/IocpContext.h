#pragma once
enum class ContextType : uint8_t
{
	Connect,
	Disconnect,
	Accept,
	Recv,
	Send,
	Notify
};

class Packet;
class Service;
class IDispatcher;
class NetworkSession;
class IocpContext : public OVERLAPPED
{
public:
	IocpContext(ContextType type);

	void Init();
public:
	ContextType mContextType;
	std::shared_ptr<IDispatcher> mDispatcher;
};

class ConnectContext : public IocpContext
{
public:
	ConnectContext() : IocpContext(ContextType::Connect) {}
};
class DisconnectContext : public IocpContext
{
public:
	DisconnectContext() : IocpContext(ContextType::Disconnect) {}
};
class RecvContext : public IocpContext
{
public:
	RecvContext() : IocpContext(ContextType::Recv) {}
};
class SendContext : public IocpContext
{
public:
	SendContext() : IocpContext(ContextType::Send) {}

	std::vector<std::shared_ptr<Packet>> mPacketList;
};

class AcceptContext : public IocpContext
{
public:
	AcceptContext() : IocpContext(ContextType::Accept) {}
};

class RIONotifyEvent : public IocpContext
{
public:
	RIONotifyEvent(HANDLE iocpHandle) : IocpContext(ContextType::Notify)
	{
		notify.Type = RIO_IOCP_COMPLETION;
		notify.Iocp.IocpHandle = iocpHandle;
		notify.Iocp.CompletionKey = (PVOID)RIO_IOCP_COMPLETION;
		notify.Iocp.Overlapped = (PVOID)this;
	}

	RIO_NOTIFICATION_COMPLETION notify = {};
	RIO_CQ rioCQ = {};
};

class RIOContext : public RIO_BUF
{
public:
	RIOContext(ContextType type) : mContextType(type) {}
	void Init() {
		BufferId = 0;
		Offset = 0;
		Length = 0;
	};

	ContextType mContextType;
	std::shared_ptr<IDispatcher> mDispatcher;
};

class RIORecvContext : public RIOContext
{
public:
	RIORecvContext() : RIOContext(ContextType::Recv) {}
};

class RIOSendContext : public RIOContext
{
public:
	RIOSendContext() : RIOContext(ContextType::Send) {}
};