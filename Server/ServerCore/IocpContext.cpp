#include "pch.h"
#include "IocpContext.h"

IocpContext::IocpContext(ContextType type)
	: mContextType(type)
{
	Init();
}

void IocpContext::Init()
{
	hEvent = 0;
	Internal = 0;
	InternalHigh = 0;
	Offset = 0;
	OffsetHigh = 0;
}