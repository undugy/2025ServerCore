#pragma once
#include "DBLoadBalancer.h"
#include "RefSingleton.h"
class GameDBLoadBalancer : public DBLoadBalancer, public RefSingleton<GameDBLoadBalancer>
{
public:
	bool Push(int32_t workId, uint32_t protocolId, std::shared_ptr<DBData> data);
	bool Push(std::shared_ptr<DBData> data);
};
