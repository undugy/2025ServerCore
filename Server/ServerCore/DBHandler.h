#pragma once
class DBData
{
public:
	DBData();
	~DBData();

	int32_t WorkID = 0;		// �뷱�� �� ID
	uint32_t ProtocolID = 0;	// ���� �� ��������

	std::shared_ptr<class JobQueue> Owner = nullptr;
	std::shared_ptr<class Job> ResponseJob = nullptr;
	void ProcessDBWorking();
};

class DBService;
using DBHandlerFunc = std::function<bool(std::shared_ptr<DBData>)>;
bool Handle_INVALID(std::shared_ptr<DBData> data);

class DBHandler
{
public:
	virtual void Init();
	/// <summary>
	/// DB �۾� ����
	/// </summary>
	/// <param name="protocolId">���� �� DB �������� ID</param>
	/// <param name="data">DB call�� �ʿ��� ������</param>
	/// <param name="service">�۾��� ������ DB ������</param>
	/// <returns></returns>
	bool HandleData(uint32_t protocolId, std::shared_ptr<DBData> data);
	/// <summary>
	/// �ڵ鷯 ���
	/// </summary>
	/// <param name="protocol">���� �� ��������</param>
	/// <param name="fn">�������� ID�� �ش��ϴ� �ڵ鷯 �Լ� ������</param>
	/// <returns></returns>
	bool RegisterHandler(const uint32_t& protocol, DBHandlerFunc fn);
	template<class HandlerType, typename = typename std::enable_if<std::is_base_of<DBHandler, HandlerType>::value>::type>
	bool RegisterHandler(const uint32_t& protocol, bool (HandlerType::* handler)(std::shared_ptr<DBData>))
	{
		return RegisterHandler(protocol, [=](std::shared_ptr<DBData> data)
			{
				return (static_cast<HandlerType*>(this)->*handler)(data);
			});
	}

private:
	std::unordered_map<uint32_t, DBHandlerFunc> mDBHandlerList;

#ifdef DEV_TEST
	std::unordered_set<uint16> _useProtocol;	// �ߺ� üũ��
#endif
};

