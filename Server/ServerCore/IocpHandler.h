#pragma once
class IDispatcher : public std::enable_shared_from_this<IDispatcher>
{
public:
	virtual HANDLE GetHandle() = 0;
	virtual void Dispatch(class IocpContext* iocpContext, int32_t numOfBytes = 0) = 0;
};

class IocpHandler
{
public:
	IocpHandler();
	~IocpHandler();

	bool RegisterHandleObject(HANDLE handle);
	bool ProcessDispatch(uint32_t timeOutMs = INFINITE);
private:
	HANDLE mIocpHandle = INVALID_HANDLE_VALUE;
};

