#include "pch.h"
#include "DBService.h"
#include "ObjectPool.h"
bool DBService::Connect(const std::string& connectionString)
{
	try
	{
		mConnectionString = connectionString;
		mSession = new Poco::Data::Session(Poco::Data::ODBC::Connector::KEY, mConnectionString, 5);
	}
	catch (Poco::Data::ConnectionFailedException& ex)
	{
		ex;
		return false;
	}

	VIEW_INFO("[DB: {}] Connect Success!!", mConnectionString);
	return true;
}

bool DBService::Push(const uint32_t& protocolId, std::shared_ptr<DBData> data, DBHandler& handler)
{
	if (data == nullptr)
		return false;

	mQueueCount.fetch_add(1);
	mDBQueue.enqueue(ObjectPool<DBQueueData>::GetInstance().New(protocolId, data, handler));
	return true;
}

void DBService::Execute()
{
	while (true)
	{
		if (mQueueCount == 0)
			return;

		std::vector<DBQueueData*> jobs(BULK_DEQUEUE_DB_COUNT);
		size_t n = mDBQueue.try_dequeue_bulk(jobs.begin(), BULK_DEQUEUE_DB_COUNT);
		int32_t jobCount = static_cast<int32_t>(n);	
		for (int32_t i = 0; i < jobCount; i++)
		{
			jobs[i]->handler.HandleData(jobs[i]->ProtocolId, jobs[i]->data);
			jobs[i]->data = nullptr;
			ObjectPool<DBQueueData>::GetInstance().Delete(jobs[i]);
		}

		if (mQueueCount.fetch_sub(jobCount) == jobCount)
			return;
	}
}
