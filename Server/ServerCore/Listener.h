#pragma once
#include "IocpHandler.h"
class ListenerService;
class AcceptContext;
class Listener : public IDispatcher
{
public:
	Listener();
	~Listener();

public:
	bool StartAccept(std::shared_ptr<ListenerService> service);
	void CloseSocket();

public:
	virtual HANDLE GetHandle() override;
	virtual void Dispatch(class IocpContext* iocpContext, int32_t numOfBytes = 0) override;
	virtual void Dispatch(class RIOContext* rioContext, int32_t numOfBytes = 0) override;
	virtual void Dispatch(class RIONotifyEvent* notifyEvent) override;

private:
	void RegisterAccept(AcceptContext* acceptContext);
	void ProcessAccept(AcceptContext* acceptContext);

protected:
	SOCKET mSocket = INVALID_SOCKET;
	std::vector<std::shared_ptr<AcceptContext>> mAcceptEvents;
	std::shared_ptr<ListenerService> mService;

};

