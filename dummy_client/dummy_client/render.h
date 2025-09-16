#pragma once
#include<array>
#include<atomic>
constexpr int MAX_ROOM_SIZE = 6;
struct TrafficCounter {
	int64_t lastTotalSent = 0, lastTotalRecv = 0;
	double emaSent = 0.0, emaRecv = 0.0;
};

extern std::array<TrafficCounter, MAX_ROOM_SIZE> g_counters;

struct RoomStats {
	std::atomic_int roomId = -1;
	std::atomic_int participants = 0;

	// 누적 바이트
	std::atomic_int64_t totalSent = 0;
	std::atomic_int64_t totalRecv = 0;

	double sentBps = 0.0; // 초당 바이트
	double recvBps = 0.0; // 초당 바이트
};

extern std::array<RoomStats, MAX_ROOM_SIZE> g_rooms;