#pragma once
#include "PacketSession.h"
#include "PacketManager.h"
using PacketHandlerFunc = std::function<bool(std::shared_ptr<PacketSession>&, BYTE*, int32_t)>;
class PacketHandler
{
public:
	virtual void Init();
	bool HandlePacket(std::shared_ptr<PacketSession>& session, BYTE* buffer, uint32_t len);
	bool RegisterHandler(const uint32_t& protocol, PacketHandlerFunc fn);
	template<class HandlerType, class PacketType, typename = typename std::enable_if<std::is_base_of<PacketHandler, HandlerType>::value>::type>
	bool RegisterHandler(const uint32_t& protocol, bool (HandlerType::* handler)(std::shared_ptr<PacketSession>&, PacketType&))
	{
		return RegisterHandler(protocol, [=](std::shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
			{
				PacketType pkt;
				if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
					return false;

				return (static_cast<HandlerType*>(this)->*handler)(session, pkt);
			});
	}
	static bool Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len);
protected:
	template<class PacketType, class ProcessFunc>
	bool HandlePacket(ProcessFunc func, std::shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
	{
		PacketType pkt;
		if (pkt.ParseFromArray(buffer + sizeof(PacketHeader), len - sizeof(PacketHeader)) == false)
			return false;

		return func(session, pkt);
	}

public:
	template<class T>
	std::shared_ptr<Packet> MakeSendBuffer(T& pkt, uint32_t pktId)
	{
		const uint32_t dataSize = static_cast<uint32_t>(pkt.ByteSizeLong());
		const uint32_t packetSize = dataSize + sizeof(PacketHeader);

		std::shared_ptr<Packet> packet = PacketManager::GetInstance().Open(packetSize);
		PacketHeader* header = reinterpret_cast<PacketHeader*>(sendBuffer->Buffer());
		header->size = packetSize;
		header->id = pktId;
		ASSERT_CRASH(pkt.SerializeToArray(&header[1], dataSize));
		packet->Close(packetSize);

		return packet;
	}

private:
	static std::unordered_map<uint32_t, PacketHandlerFunc> mPacketHandler;

};

