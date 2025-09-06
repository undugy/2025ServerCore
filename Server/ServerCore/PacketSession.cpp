#include "pch.h"
#include "PacketSession.h"
PacketSession::PacketSession()
{

}

PacketSession::~PacketSession()
{

}

int32_t PacketSession::OnRecv(BYTE* buffer, int32_t len)
{
	int32_t processLen = 0;

	while (true)
	{
		int32_t dataSize = len - processLen;
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *reinterpret_cast<PacketHeader*>(&buffer[processLen]);
		if (dataSize < header.size)
			break;

		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}