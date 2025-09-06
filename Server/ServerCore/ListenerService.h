#pragma once
#include "Service.h"
class Listener;
class ListenerService : public Service
{
public:
	ListenerService(SocketAddress targetAddress, std::shared_ptr<IocpHandler> iocpHandler, std::function<std::shared_ptr<class NetworkSession>(void)> consturctor, int32_t maxSessionCount = 1);
	virtual ~ListenerService() {}

	virtual bool Start() override;
	virtual void CloseService() override;

	bool CreateRIOCQ();

private:
	std::shared_ptr<Listener> mListener = nullptr;

};

