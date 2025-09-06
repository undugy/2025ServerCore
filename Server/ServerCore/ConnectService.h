#pragma once
#include "Service.h"
class ConnectService : public Service
{
public:
	ConnectService(SocketAddress targetAddress, std::shared_ptr<IocpHandler> iocpHandler, std::function<std::shared_ptr<class NetworkSession>(void)> consturctor, int32_t maxSessionCount = 1);
	virtual ~ConnectService() {}

	virtual bool Start() override;
};

