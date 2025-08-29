#pragma once
#include"IocpHandler.h"
class Service;
class SendBuffer;
class IocpContext;
class NetworkSession : public IDispatcher
{
	int32_t mSessionID = 0;
	std::atomic_bool mIsConnected = false;
	SOCKADDR_IN mSockAddr;
	SOCKET mSocket = INVALID_SOCKET;
	
	std::weak_ptr<Service> mService;
	moodycamel::ConcurrentQueue<std::shared_ptr<SendBuffer>> mSendQueue;
public:
	NetworkSession();
	virtual ~NetworkSession();
public:
	void Send(std::shared_ptr<SendBuffer> buffer);
	bool Connect();
	void Disconnect(const WCHAR* cause);

	std::shared_ptr<Service> GetService();
	void SetService(std::shared_ptr<Service> service);

	void SetNetAddr(const SOCKADDR_IN& address);
	const SOCKADDR_IN& GetAddress() const;

	const SOCKET& GetSocket()const;
	bool IsConnected(); 
	int32_t GetSessionID();
	std::shared_ptr<NetworkSession> GetSession();
private:
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(IocpContext* iocpContext, int32_t numOfBytes = 0) override;

};
