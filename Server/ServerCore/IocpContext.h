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

class Session;
class SendBuffer;
class Service;
class IDispatcher;
class IocpContext : public OVERLAPPED
{
public:
	IocpContext(ContextType type);

	void Init();
public:
	ContextType mContextType;
	std::shared_ptr<Service> mService;
	std::shared_ptr<IDispatcher> mDispatcher;
};


