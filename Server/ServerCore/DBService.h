#pragma once
#include "DBHandler.h"
#include "PocoTimeUtil.h"

#include <Poco/Data/RecordSet.h>
#include <Poco/Data/ODBC/ODBCException.h>
#include <Poco/Data/ODBC/ODBCStatementImpl.h>

#pragma comment (lib, "iphlpapi.lib")	// GetAdaptersInfo()
using namespace Poco::Data::Keywords;		// in() out()
constexpr int32_t MAX_DB_CONNECTION_RETRY = 5;	// DB 연결 재시도 횟수
constexpr int32_t BULK_DEQUEUE_DB_COUNT = 300;	// 한 번에 처리할 DB 작업 개수
struct DBQueueData 
{
	uint32_t ProtocolId = 0;
	std::shared_ptr<DBData> data = nullptr;
	DBHandler& handler;
};

class DBService
{
public:
	/// <summary>
	/// DB 연결 직접호출 하지 않고 로드밸런서를 통해 호출
	/// </summary>
	/// <param name="connectionString"></param>
	/// <returns></returns>
	bool Connect(const std::string& connectionString);
	/// <summary>
	/// DB 작업 Push 직접호출 하지 않고 로드밸런서를 통해 호출
	/// </summary>
	/// <param name="protocolId"></param>
	/// <param name="data"></param>
	/// <param name="handler"></param>
	/// <returns></returns>
	bool Push(const uint32_t& protocolId, std::shared_ptr<DBData> data, DBHandler& handler);
	/// <summary>
	/// 해당 스레드에 배정된 DB 작업을 처리
	/// </summary>
	void Execute();

	Poco::Data::Session* GetDBSession() { return mSession; }


private:
	std::string mConnectionString;
	Poco::Data::Session* mSession = nullptr;

	std::atomic_int32_t mQueueCount = 0;
	moodycamel::ConcurrentQueue<DBQueueData*> mDBQueue;	// DB 작업 큐
};
