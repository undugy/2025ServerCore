#pragma once
#include "DBService.h"

constexpr uint64_t DBRECONNECTTIME = 5000ll;	// DB �翬�� �ð�

class DBLoadBalancer
{
public:
	/// <summary>
	/// DB ����
	/// </summary>
	/// <param name="connectionString">������ ���� ��Ʈ��, ���̵� ��й�ȣ�� �ʿ��� ��� ���� �ϳ� �� ��������</param>
	/// <param name="serviceCount">DB ������ ����</param>
	/// <returns></returns>
	bool Init(const std::string& connectionString, const int32_t& serviceCount);
	bool Init(const std::string& driver, const std::string& id, const std::string& pwd, const std::string& db, const std::string& host, const std::string& port, const int32_t& serviceCount);
	/// <summary>
	/// �ε�뷱���� DB �۾� Push
	/// </summary>
	/// <param name="workId">�뷱���� ID</param>
	/// <param name="protocolId">��������</param>
	/// <param name="data">DB�����忡 �ѱ� ������, DBData�� ��� ���� �����͸� ���</param>
	/// <param name="handler">�۾��� ������ �ڵ鷯</param>
	/// <returns></returns>
	bool Push(int32_t workId, uint32_t protocolId, std::shared_ptr<DBData> data, DBHandler& handler);
	bool Push(std::shared_ptr<DBData> data, DBHandler& handler);

	void Launch();

	DBService& GetDBService(int32_t workId);

private:
	std::string mConnectionString = "";
	DBService*	mServiceList = nullptr;	// �迭�� ���
	int32_t		mServiceCount = 0;
};

