#pragma once
#include "pch.h"

extern thread_local int32_t LThreadId;
extern thread_local int64_t LEndTickCount;
extern thread_local std::shared_ptr <class PacketChunk> LPacketChunk;
extern thread_local class JobQueue* LCurrentJobQueue;