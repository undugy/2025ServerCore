#include "pch.h"
#include "ServerConfig.h"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
bool ServerConfig::LoadConfig(const std::string& configFile)
{
    if (true == mIsLoaded)
        return false;
	std::error_code ec;
	auto currentDir = fs::current_path(ec);
	if (ec.value() != 0)
		return false;	// 에러 났어요...

	std::string strPath = {};
	strPath += "\\";
	strPath += configFile;
	currentDir.append(strPath);

	auto filePath = fs::path(strPath);
	if (false == fs::exists(filePath, ec))
		return false;
	if (false == fs::is_regular_file(filePath, ec))
		return false;

	auto fileSize = fs::file_size(filePath, ec);
	if (ec.value() != 0)
		return false;

	mStrConfigFileName = configFile;

	mIsLoaded = _LoadConfig(strPath, fileSize);
	_LoadCPUCount();
	return mIsLoaded;
}

bool ServerConfig::_LoadConfig(const std::string& configFile, size_t fileSize)
{
	if (true == configFile.empty())
		return false;

	if (fileSize == 0
		|| fileSize == (uintmax_t)~0)
		return false;

	std::ifstream is(configFile,std::ios::binary);
	if (false == is.is_open())
		return false;

	if (true == is.fail())
	{
		VIEW_ERROR("Config Read Fail");
		return false;
	}


	std::string json((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());


	is.close();

	rapidjson::Document doc;
	doc.Parse(json.c_str());
	//컨피그 읽기
	rapidjson::Value& mainValue = doc["MainListener"];
	mServerConfigData.ProcessName = mainValue["ProcessName"].GetString();
	mServerConfigData.MaxConnectCount = mainValue["MaxConnectCount"].GetInt();
	mServerConfigData.BindPort = mainValue["BindPort"].GetInt();
	mServerConfigData.NetworkThreadCount = mainValue["NetworkThreadCount"].GetInt();
	mServerConfigData.BindAddr = mainValue["BindAddr"].GetString();

	//DB 읽기
	rapidjson::Value& dbValue = doc["DB"];
	for (rapidjson::Value::ConstMemberIterator itr = dbValue.MemberBegin(); itr != dbValue.MemberEnd(); ++itr)
	{
		DBConfigData dbConfig;
		dbConfig.UID = dbValue[itr->name.GetString()]["UID"].GetString();
		dbConfig.PWD = dbValue[itr->name.GetString()]["PWD"].GetString();
		dbConfig.DATABASE = dbValue[itr->name.GetString()]["DATABASE"].GetString();
		dbConfig.HOST = dbValue[itr->name.GetString()]["HOST"].GetString();
		dbConfig.PORT = dbValue[itr->name.GetString()]["PORT"].GetInt();
		dbConfig.THREAD_COUNT = dbValue[itr->name.GetString()]["THREAD_COUNT"].GetInt();
		mDBConfigDataMap.insert({ itr->name.GetString(),dbConfig });
	}

	//ObjectPool 읽기
	rapidjson::Value& objectPoolValue = doc["ObjectPool"];
	mObjectPoolConfig.Packet = objectPoolValue["Packet"].GetInt();
	mObjectPoolConfig.DBPacket = objectPoolValue["DBPacket"].GetInt();
	mObjectPoolConfig.Job = objectPoolValue["Job"].GetInt();
	return true;
}

bool ServerConfig::_LoadCPUCount()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	mServerConfigData.ProcessorCount = (int32_t)info.dwNumberOfProcessors;

	if (mServerConfigData.ProcessorCount <= 0)
		mServerConfigData.ProcessorCount = 1;
	return true;
}
