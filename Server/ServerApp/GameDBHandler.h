#pragma once
#include "DBHandler.h"
#include "RefSingleton.h"

#define GetSession()		if (data == nullptr)																						\
								return false;																							\
							Poco::Data::Session* dbSession = GameDBLoadBalancer::GetInstance().GetDBService(data->WorkID).GetDBSession();	\
							if (dbSession == nullptr)																					\
								return false;																							\
							Poco::Data::Session& session = *dbSession;

class GameDBHandler : public DBHandler, public RefSingleton<GameDBHandler>
{
public:
	void Init() override;

	bool OnSGDBServerStart(std::shared_ptr<DBData> data);
	bool OnSGDBChatRequest(std::shared_ptr<DBData> data);
};

