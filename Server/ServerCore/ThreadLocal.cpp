#include "pch.h"
#include "ThreadLocal.h"


thread_local int32_t LThreadId = 0;
thread_local int64_t LEndTickCount = 0LL;
thread_local std::shared_ptr <PacketChunk> LSendBufferChunk;