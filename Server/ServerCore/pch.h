#pragma once

// pch.h: 미리 컴파일된 헤더 파일입니다.
// 아래 나열된 파일은 한 번만 컴파일되었으며, 향후 빌드에 대한 빌드 성능을 향상합니다.
// 코드 컴파일 및 여러 코드 검색 기능을 포함하여 IntelliSense 성능에도 영향을 미칩니다.
// 그러나 여기에 나열된 파일은 빌드 간 업데이트되는 경우 모두 다시 컴파일됩니다.
// 여기에 자주 업데이트할 파일을 추가하지 마세요. 그러면 성능이 저하됩니다.

#ifndef PCH_H
#define PCH_H
#define VERSION_RIO

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#pragma warning(disable : 26498 4819 6386 26800)	  // SPD Log
#pragma warning (disable: 26439 26498 26451 26444 26451 26812)	// VC++ Code Guidelines warning. POCO 라이브러리가 문제...
// 여기에 미리 컴파일하려는 헤더 추가
#include <iostream>
#include <shared_mutex>
#include <thread>
#include <functional>
#include <format>
#include <chrono>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <mimalloc.h>
#include <memory>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mimalloc.dll.lib")

#pragma comment(lib, "Poco\\PocoDatamd.lib")
#pragma comment(lib, "Poco\\PocoDataODBCmd.lib")
#pragma comment(lib, "Poco\\PocoFoundationmd.lib")

#include<modycamel/concurrentqueue.h>

#include"ObjectPool.h"
#include "LogManager.h"
#include "PocoTimeUtil.h"
#include "LogMacro.h"
#include "PacketChunk.h"
#include "ThreadLocal.h"
#endif //PCH_H