#pragma once
constexpr int RIO_DISPATCH_RESULT_COUNT = 256;
class IDispatcher : public std::enable_shared_from_this<IDispatcher>
{
public:
	virtual HANDLE GetHandle() = 0;
	virtual void Dispatch(class IocpContext* iocpContext, int32_t numOfBytes = 0) = 0;
	virtual void Dispatch(class RIOContext* rioEvent, int32_t numOfBytes = 0) = 0;
	virtual void Dispatch(class RIONotifyEvent* notifyEvent) {};
};

class IocpHandler
{
public:
	IocpHandler();
	~IocpHandler();
	HANDLE GetHandle() { return mIocpHandle; }
	bool RegisterHandleObject(HANDLE handle);
	bool ProcessDispatch(uint32_t timeOutMs = INFINITE);
private:
	HANDLE mIocpHandle = INVALID_HANDLE_VALUE;
};

