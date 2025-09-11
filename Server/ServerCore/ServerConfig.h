#pragma once
#include"RefSingleton.h"
struct ServerConfigData
{
	std::string ProcessName = "";
	int32_t MaxConnectCount = 0;
	int32_t BindPort = 0;
	int32_t NetworkThreadCount = 0;
	int32_t ProcessorCount = 0;
	std::string BindAddr = "";
};

struct DBConfigData
{
	std::string UID = "";
	std::string PWD = "";
	std::string DATABASE = "";
	std::string HOST = "";
	int32_t PORT = 0;
	int32_t THREAD_COUNT = 0;
};

struct ObjectPoolConfig
{
	int32_t Packet = 0;
	int32_t DBPacket = 0;
	int32_t Job = 0;
};

class ServerConfig : public RefSingleton<ServerConfig>
{
private:
	std::atomic_bool mIsLoaded = false;
	std::string mStrConfigFileName = "";
	ServerConfigData mServerConfigData;
	std::unordered_map<std::string, DBConfigData> mDBConfigDataMap;
	ObjectPoolConfig mObjectPoolConfig;
public:
	bool LoadConfig(const std::string& configFile = "ServerConfig.json");
	const ServerConfigData& GetServerConfigData() const { return mServerConfigData; }
	const std::unordered_map<std::string, DBConfigData>& GetDBConfigDataMap() const { return mDBConfigDataMap; }
	const ObjectPoolConfig& GetObjectPoolConfig() const { return mObjectPoolConfig; }
private:
	bool _LoadConfig(const std::string& configFile, size_t fileSize);
	bool _LoadCPUCount();
};

