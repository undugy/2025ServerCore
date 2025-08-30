#pragma once
class SocketAddress
{
public:
	SocketAddress() = default;
	SocketAddress(SOCKADDR_IN sockAddr);
	SocketAddress(std::wstring ip, uint16_t port);

	SOCKADDR_IN& GetSockAddr() { return _sockAddr; }
	std::wstring GetIpAddress();
	uint16_t GetPort() { return ::ntohs(_sockAddr.sin_port); }

public:
	static IN_ADDR Ip2Address(const WCHAR* ip);

private:
	SOCKADDR_IN _sockAddr = {};
};

class SocketUtil
{
public:
	static LPFN_CONNECTEX ConnectEx;
	static LPFN_DISCONNECTEX DisconnectEx;
	static LPFN_ACCEPTEX AcceptEx;
	static RIO_EXTENSION_FUNCTION_TABLE RIOEFTable;

public:
	static bool Init();
	static void Clear();

	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
	static SOCKET CreateSocket();

	static bool SetLinger(SOCKET socket, uint16_t onoff, uint16_t linger);
	static bool SetReuseAddress(SOCKET socket, bool flag);
	static bool SetRecvBufferSize(SOCKET socket, int32_t size);
	static bool SetSendBufferSize(SOCKET socket, int32_t size);
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);

	static bool Bind(SOCKET socket, SocketAddress netAddr);
	static bool BindAnyAddress(SOCKET socket, uint16_t port);
	static bool Listen(SOCKET socket, int32_t backlog = SOMAXCONN);
	static void Close(SOCKET& socket);
};

template<class T>
static inline bool SetSocketOpt(SOCKET socket, int32_t level, int32_t optName, T optVal)
{
	return ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T)) != SOCKET_ERROR;
}