#pragma once
#include"IocpHandler.h"
#include"IocpContext.h"
#include"SocketUtil.h"
#include"NetworkBuffer.h"
#include"CustomBuffer.h"
class Service;
class SendBuffer;
class IocpContext;
class NetworkBuffer;
class CustomBuffer;

constexpr int32_t OUTSTANDING_COUNT = 16;
constexpr int32_t BULK_DEQUEUE_COUNT = 500;
constexpr int32_t WARN_SENDQUEUE_SIZE = 500;
constexpr int32_t KICK_SENDQUEUE_SIZE = 1000;
class NetworkSession : public IDispatcher
{
	friend class IocpCore;
	friend class Service;
	friend class Listener;

	int32_t mSessionID = 0;
	std::atomic_bool mIsConnected = false;
	SocketAddress mSockAddr = {};
	SOCKET mSocket = INVALID_SOCKET;
	
	std::weak_ptr<Service> mService;
	std::shared_ptr<NetworkBuffer> mRecvBuffer = nullptr;
	std::shared_ptr<NetworkBuffer> mSendBuffer = nullptr;
	CustomBuffer mCustomBuffer;
	moodycamel::ConcurrentQueue<std::shared_ptr<Packet>> mSendQueue;
	std::atomic_int mPacketCount = 0;
	std::atomic_bool mSendRegistered = false;

	// RIO
	RIO_RQ mRioRQ;
	std::recursive_mutex mRQLock;
	RIORecvContext mRioRecvContext;
	RIOSendContext mRioSendContext;

	ConnectContext mConnectContext;
	DisconnectContext mDisconnectContext;

	RecvContext mRecvContext;
	SendContext mSendContext;
public:
	NetworkSession();
	virtual ~NetworkSession();
public:
	void Send(std::shared_ptr<Packet> buffer);
	bool Connect();
	void Disconnect(const WCHAR* cause);

	std::shared_ptr<Service> GetService();
	void SetService(std::shared_ptr<Service> service);

	void SetNetAddr(const SocketAddress& address);
	const SocketAddress& GetAddress() const;

	const SOCKET& GetSocket()const;
	bool IsConnected(); 
	int32_t GetSessionID();
	std::shared_ptr<NetworkSession> GetSession();

	bool RegisterRIOBuffer();
	bool CreateRIORQ();

public: //IDispatcher
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(IocpContext* iocpContext, int32_t numOfBytes = 0) override;
	virtual void Dispatch(class RIOContext* rioContext, int32_t numOfBytes = 0) override;
	virtual void Dispatch(class RIONotifyEvent* notifyEvent) override;
private:
	void SetSessionID();

	bool RegisterConnect();
	bool RegisterDisconnect();
	void RegisterRecv();
	void RegisterSend();

	void ProcessConnect();
	void ProcessDisconnect();
	void ProcessRecv(int32_t numOfBytes);
	void ProcessSend(int32_t numOfBytes);

	void HandleError(int32_t errorCode);

protected:
	virtual void OnConnected([[maybe_unused]] const SocketAddress& sockAddr) {}
	virtual int32_t OnRecv([[maybe_unused]] BYTE* buffer, int32_t len) { return len; }
	virtual void OnSend([[maybe_unused]] int32_t len) {}
	virtual void OnDisconnected() {}

public:
	template<typename T>
	static std::shared_ptr<T> GetSessionConstructor()
	{
		return ObjectPool<T>::GetInstance().Acquire();
	}
};
