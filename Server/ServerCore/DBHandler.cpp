#include "pch.h"
#include "DBHandler.h"
#include "DBService.h"
#include "JobQueue.h"

DBData::DBData()
{

}

DBData::~DBData()
{

}

void DBData::ProcessDBWorking()
{
	if (Owner == nullptr || ResponseJob == nullptr)
	{
		Owner = nullptr;
		ResponseJob = nullptr;
		return;
	}

	Owner->Push(ResponseJob, true);
	ResponseJob = nullptr;
}

void DBHandler::Init()
{
#ifdef DEV_TEST
	_useProtocol.clear();
#endif
}

bool DBHandler::HandleData(uint32_t protocolId, std::shared_ptr<DBData> data)
{
	return mDBHandlerList[protocolId](data);
}

bool DBHandler::RegisterHandler(const uint32_t& protocol, DBHandlerFunc fn)
{
	if (fn == nullptr)
		return false;
#ifdef DEV_TEST
	if (_useProtocol.insert(protocol).second == false)
	{
		VIEW_WRITE_ERROR("Duplicated Protocol Detected! Check Protocol ID({})", protocol);
		return false;
	}
#endif
	mDBHandlerList[protocol] = fn;
	return true;
}

bool Handle_INVALID(std::shared_ptr<DBData> data)
{
	if (data == nullptr)
		return false;
	VIEW_WRITE_ERROR("Invalid Inner Data dected ProtocolID({}), WorkID({})", data->ProtocolID, data->WorkID);
	return false;
}
