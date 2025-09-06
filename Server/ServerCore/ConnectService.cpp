#include "pch.h"
#include "ConnectService.h"
#include "NetworkSession.h"
ConnectService::ConnectService(SocketAddress targetAddress, std::shared_ptr<IocpHandler> iocpHandler, std::function<std::shared_ptr<class NetworkSession>(void)> consturctor, int32_t maxSessionCount)
	: Service(ServiceType::Connector, targetAddress, iocpHandler, consturctor, maxSessionCount)
{
}

bool ConnectService::Start()
{
	if (CanStart() == false)
		return false;

	const int32_t sessionCount = GetMaxSessionCount();
	for (int32_t i = 0; i < sessionCount; i++)
	{
		std::shared_ptr<NetworkSession> session = CreateSession();
		if (session->Connect() == false)
			return false;
	}

	return true;
}
