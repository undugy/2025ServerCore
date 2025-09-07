#include "pch.h"
#include "DBLoadBalancer.h"
#include "ThreadManager.h"

bool DBLoadBalancer::Init(const std::string& connectionString, const int32_t& serviceCount)
{
	if (connectionString.empty() || serviceCount <= 0)
		return false;

	mConnectionString = connectionString;
	Poco::Data::ODBC::Connector::registerConnector();
	mServiceList = new DBService[serviceCount]();
	for (int32_t i = 0; i < serviceCount; i++)
	{
		if (mServiceList[i].Connect(connectionString) == false)
			return false;
	}
	mServiceCount = serviceCount;
	return true;
}

bool DBLoadBalancer::Init(const std::string& driver, const std::string& id, const std::string& pwd, const std::string& db, const std::string& host, const std::string& port, const int32_t& serviceCount)
{
	std::string connectionString = std::format("DRIVER={};UID={};PWD={};DATABASE={};SERVER={},{};", driver, id, pwd, db, host, port);
	return Init(connectionString, serviceCount);
}

bool DBLoadBalancer::Push(int32_t workId, uint32_t protocolId, std::shared_ptr<DBData> data, DBHandler& handler)
{
	if (mServiceList == nullptr)
		return false;
	GetDBService(workId).Push(protocolId, data, handler);
	return true;
}

bool DBLoadBalancer::Push(std::shared_ptr<DBData> data, DBHandler& handler)
{
	if (data == nullptr)
		return false;

	return Push(data->WorkID, data->ProtocolID, data, handler);
}

void DBLoadBalancer::Launch()
{
	for (int32_t i = 0; i < mServiceCount; i++)
	{
		DBService& service = mServiceList[i];
		ThreadManager::GetInstance().PushThread([&]()
			{
				while (true)
				{
					if (service.GetDBSession() == nullptr)
						continue;

					if (service.GetDBSession()->isConnected() == false && LEndTickCount < PocoTimeUtil::GetTick64())
					{
						service.Connect(mConnectionString);
						LEndTickCount = PocoTimeUtil::GetTick64() + DBRECONNECTTIME;
						continue;
					}

					service.Execute();

					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			});
	}
}

DBService& DBLoadBalancer::GetDBService(int32_t workId)
{
	return mServiceList[workId % mServiceCount];
}
