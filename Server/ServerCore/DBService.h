#pragma once
#include "DBHandler.h"
#include "PocoTimeUtil.h"

#include <Poco/Data/RecordSet.h>
#include <Poco/Data/ODBC/ODBCException.h>
#include <Poco/Data/ODBC/ODBCStatementImpl.h>

#pragma comment (lib, "iphlpapi.lib")	// GetAdaptersInfo()
using namespace Poco::Data::Keywords;		// in() out()
constexpr int32_t MAX_DB_CONNECTION_RETRY = 5;	// DB ���� ��õ� Ƚ��
constexpr int32_t BULK_DEQUEUE_DB_COUNT = 300;	// �� ���� ó���� DB �۾� ����
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
	/// DB ���� ����ȣ�� ���� �ʰ� �ε�뷱���� ���� ȣ��
	/// </summary>
	/// <param name="connectionString"></param>
	/// <returns></returns>
	bool Connect(const std::string& connectionString);
	/// <summary>
	/// DB �۾� Push ����ȣ�� ���� �ʰ� �ε�뷱���� ���� ȣ��
	/// </summary>
	/// <param name="protocolId"></param>
	/// <param name="data"></param>
	/// <param name="handler"></param>
	/// <returns></returns>
	bool Push(const uint32_t& protocolId, std::shared_ptr<DBData> data, DBHandler& handler);
	/// <summary>
	/// �ش� �����忡 ������ DB �۾��� ó��
	/// </summary>
	void Execute();

	Poco::Data::Session* GetDBSession() { return mSession; }


private:
	std::string mConnectionString;
	Poco::Data::Session* mSession = nullptr;

	std::atomic_int32_t mQueueCount = 0;
	moodycamel::ConcurrentQueue<DBQueueData*> mDBQueue;	// DB �۾� ť
};
