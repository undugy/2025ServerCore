#include "pch.h"
#include "SocketUtil.h"

SocketAddress::SocketAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr)
{
}

SocketAddress::SocketAddress(std::wstring ip, uint16_t port)
{
	::memset(&_sockAddr, 0, sizeof(_sockAddr));
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr = Ip2Address(ip.c_str());
	_sockAddr.sin_port = ::htons(port);
}

std::wstring SocketAddress::GetIpAddress()
{
	WCHAR buffer[100];
	int32_t len = static_cast<int32_t>(sizeof(buffer) / sizeof(buffer[0]));
	::InetNtopW(AF_INET, &_sockAddr.sin_addr, buffer, len);
	return std::wstring(buffer);
}

IN_ADDR SocketAddress::Ip2Address(const WCHAR* ip)
{
	IN_ADDR address;
	::InetPtonW(AF_INET, ip, &address);
	return address;
}

LPFN_CONNECTEX SocketUtil::ConnectEx;
LPFN_DISCONNECTEX SocketUtil::DisconnectEx;
LPFN_ACCEPTEX SocketUtil::AcceptEx;
RIO_EXTENSION_FUNCTION_TABLE SocketUtil::RIOEFTable;
bool SocketUtil::Init()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), OUT & wsaData) != S_OK)
		return false;
	SOCKET dummySocket = CreateSocket();
	if(BindWindowsFunction(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)))
	{
		Close(dummySocket);
		return false;
	}
	if(BindWindowsFunction(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisconnectEx)))
	{
		Close(dummySocket);
		return false;
	}
	if(BindWindowsFunction(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)))
	{
		Close(dummySocket);
		return false;
	}
#ifdef VERSION_RIO
	GUID guid = WSAID_MULTIPLE_RIO;
	DWORD bytes = 0;
	if(WSAIoctl(dummySocket, SIO_GET_MULTIPLE_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), (void**)&RIOEFTable, sizeof(RIOEFTable), OUT & bytes, NULL, NULL) != SOCKET_ERROR))
	{
		Close(dummySocket);
		return false;
	}
#endif
	Close(dummySocket);
}
void SocketUtil::Clear()
{
	WSACleanup();
}

bool SocketUtil::BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn)
{
	DWORD bytes = 0;
	return ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), fn, sizeof(*fn), OUT & bytes, NULL, NULL) != SOCKET_ERROR;
}

SOCKET SocketUtil::CreateSocket()
{
#ifdef VERSION_RIO
	return ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_REGISTERED_IO);
#else
	return ::WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
#endif
}

bool SocketUtil::SetLinger(SOCKET socket, uint16_t onoff, uint16_t linger)
{
	LINGER option;
	option.l_onoff = onoff;
	option.l_linger = linger;
	return SetSocketOpt(socket, SOL_SOCKET, SO_LINGER, option);
}

bool SocketUtil::SetReuseAddress(SOCKET socket, bool flag)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketUtil::SetRecvBufferSize(SOCKET socket, int32_t size)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_RCVBUF, size);
}

bool SocketUtil::SetSendBufferSize(SOCKET socket, int32_t size)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_SNDBUF, size);
}

bool SocketUtil::SetTcpNoDelay(SOCKET socket, bool flag)
{
	return SetSocketOpt(socket, IPPROTO_TCP, TCP_NODELAY, flag);
}

bool SocketUtil::SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket)
{
	return SetSocketOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

bool SocketUtil::Bind(SOCKET socket, SocketAddress netAddr)
{
	return ::bind(socket, reinterpret_cast<const SOCKADDR*>(&netAddr.GetSockAddr()), sizeof(SOCKADDR_IN)) != SOCKET_ERROR;
}

bool SocketUtil::BindAnyAddress(SOCKET socket, uint16_t port)
{
	SOCKADDR_IN myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = ::htonl(INADDR_ANY);
	myAddress.sin_port = ::htons(port);

	return ::bind(socket, reinterpret_cast<const SOCKADDR*>(&myAddress), sizeof(SOCKADDR_IN)) != SOCKET_ERROR;
}

bool SocketUtil::Listen(SOCKET socket, int32_t backlog)
{
	return ::listen(socket, backlog) != SOCKET_ERROR;
}

void SocketUtil::Close(SOCKET& socket)
{
	if (socket != INVALID_SOCKET)
		::closesocket(socket);
	socket = INVALID_SOCKET;
}
