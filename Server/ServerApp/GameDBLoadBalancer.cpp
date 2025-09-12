#include "ServerPch.h"
#include "GameDBLoadBalancer.h"
#include "GameDBHandler.h"

bool GameDBLoadBalancer::Push(int32_t workId, uint32_t protocolId, std::shared_ptr<DBData> data)
{
	if (data != nullptr)
	{
		data->WorkID = workId;
		data->ProtocolID = protocolId;
	}
	return DBLoadBalancer::Push(workId, protocolId, data, GameDBHandler::GetInstance());
}

bool GameDBLoadBalancer::Push(std::shared_ptr<DBData> data)
{
	if (data != nullptr && data->ProtocolID == 0)
		VIEW_WRITE_ERROR("DBData ProtocolID is zero!!");
	return DBLoadBalancer::Push(data, GameDBHandler::GetInstance());
}
