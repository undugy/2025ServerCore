#include "ServerPch.h"
#include "ServerApp.h"
#include "RegisteredBufferPool.h"
#include "NetworkStatistics.h"
bool ServerApp::Init()
{
	ThreadManager::GetInstance();	// 메인스레드 ID 생성 위해
	if (ServerConfig::GetInstance().LoadConfig("ServerConfig.json") == false)
		return false;

	if (false == SocketUtil::Init())
		return false;

	auto serverConfig = ServerConfig::GetInstance().GetServerConfigData();
	LogManager::GetInstance().Initialize(serverConfig.ProcessName);
	GameDBHandler::GetInstance().Init();
	ClientPacketHandler::GetInstance().Init();
	//if(InitGameDB() == false)
		//return false;

	if(InitListener() == false)
		return false;

	InitObjectPool();
	return true;
}

bool ServerApp::Update()
{
	LogManager::GetInstance().Launch();
	//GameDBLoadBalancer::GetInstance().Launch();
	std::shared_ptr<DBData> data = ObjectPool<DBData>::GetInstance().Acquire();
	data->ProtocolID = EDBProtocol::SGDB_ServerStart;
	//GameDBLoadBalancer::GetInstance().Push(std::move(data));
	NetworkStatistics::GetInstance().SetIsRunning(true);
	std::shared_ptr<TimerJobQueue> jobQueue = std::make_shared<TimerJobQueue>();
	jobQueue->UpdateTime();
	while (true)
	{
		LEndTickCount = PocoTimeUtil::GetTick64() + 64;
		ThreadManager::GetInstance().DistributeJobs();
		ThreadManager::GetInstance().DoGlobalJobs();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return true;
}

bool ServerApp::InitListener()
{
	std::shared_ptr<ListenerService> listenerService = ObjectPool<ListenerService>::GetInstance().Acquire(
		SocketAddress(L"0.0.0.0", 9999),
		ObjectPool<IocpHandler>::GetInstance().Acquire(),
		&NetworkSession::GetSessionConstructor<ClientSession>,
		ServerConfig::GetInstance().GetServerConfigData().MaxConnectCount);
	if(false == listenerService->Start())
		return false;
#ifdef VERSION_RIO
	for (int32_t i = 0; i < ServerConfig::GetInstance().GetServerConfigData().NetworkThreadCount; i++)
	{
		if (listenerService->CreateRIOCQ() == false)
			return false;
	}
#endif
	return true;
}

bool ServerApp::InitWorkerThread(std::shared_ptr<ListenerService>& service)
{
	if(service == nullptr)
		return false;

	for(int32_t i = 0; i < ServerConfig::GetInstance().GetServerConfigData().ProcessorCount; i++)
	{
		ThreadManager::GetInstance().PushThread([service]() {
			while (true) {
				service->GetIocpHandler()->ProcessDispatch(10);
				ThreadManager::GetInstance().DistributeJobs();
				ThreadManager::GetInstance().DoGlobalJobs();

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			});
	}
	return true;
}

bool ServerApp::InitGameDB()
{
	auto dbConfigIt = ServerConfig::GetInstance().GetDBConfigDataMap().find("GameDB");
	if(dbConfigIt == ServerConfig::GetInstance().GetDBConfigDataMap().end())
		return false;

	auto& dbConfig = dbConfigIt->second;
	auto connectStr = std::format("DRIVER=SQL SERVER;UID={};PWD={};DATABASE={};SERVER={},{};"
		, dbConfig.UID, dbConfig.PWD, dbConfig.DATABASE, dbConfig.HOST, dbConfig.PORT);
	if (GameDBLoadBalancer::GetInstance().Init(
		connectStr
		, dbConfig.THREAD_COUNT) == false)
		return false;
	return true;
}

bool ServerApp::InitObjectPool()
{
	auto& objectPoolConfig = ServerConfig::GetInstance().GetObjectPoolConfig();	
	auto& serverConfig = ServerConfig::GetInstance().GetServerConfigData();
	RegisteredBufferPool<RECV_BUFFER_SIZE>::GetInstance().Initialize(serverConfig.MaxConnectCount);
	RegisteredBufferPool<SEND_BUFFER_SIZE>::GetInstance().Initialize(serverConfig.MaxConnectCount);

	ObjectPool<Job>::GetInstance().Reserve(objectPoolConfig.Job ,nullptr);
	ObjectPool<DBQueueData>::GetInstance().Reserve(static_cast<size_t>(objectPoolConfig.DBPacket), 0, nullptr, GameDBHandler::GetInstance());

	return true;
}

void TimerJobQueue::UpdateTime()
{
	NetworkStatistics::GetInstance().PrintStatistics();
	DoTimer(100000, &TimerJobQueue::UpdateTime);
}
