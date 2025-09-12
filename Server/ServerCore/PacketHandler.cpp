#include "pch.h"
#include "PacketHandler.h"

void PacketHandler::Init()
{

}

bool PacketHandler::HandlePacket(std::shared_ptr<PacketSession>& session, BYTE* buffer, uint32_t len)
{
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
	auto it = mPacketHandler.find(header->id);
    if(it == mPacketHandler.end())
    {
        return PacketHandler::Handle_INVALID(session, buffer, len);
	}
    it->second(session, buffer, len);
    return true;
}

bool PacketHandler::RegisterHandler(const uint32_t& protocol, PacketHandlerFunc fn)
{
    if (fn == nullptr)
        return false;
    mPacketHandler[protocol] = fn;
    return true;
}

bool PacketHandler::Handle_INVALID(std::shared_ptr<PacketSession>& session, BYTE* buffer, int32_t len)
{
    PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);
    if (header == nullptr)
        return false;

    VIEW_WRITE_ERROR("Invalid Pakcet dected ID({})", header->id);
    return false;
}
