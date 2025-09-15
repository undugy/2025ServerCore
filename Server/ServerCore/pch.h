#pragma once

// pch.h: �̸� �����ϵ� ��� �����Դϴ�.
// �Ʒ� ������ ������ �� ���� �����ϵǾ�����, ���� ���忡 ���� ���� ������ ����մϴ�.
// �ڵ� ������ �� ���� �ڵ� �˻� ����� �����Ͽ� IntelliSense ���ɿ��� ������ ��Ĩ�ϴ�.
// �׷��� ���⿡ ������ ������ ���� �� ������Ʈ�Ǵ� ��� ��� �ٽ� �����ϵ˴ϴ�.
// ���⿡ ���� ������Ʈ�� ������ �߰����� ������. �׷��� ������ ���ϵ˴ϴ�.

#ifndef PCH_H
#define PCH_H
#define VERSION_RIO
#ifndef POCO_STATIC
#define POCO_STATIC 1
#endif
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#pragma warning(disable : 26498 4819)	  // spdlog
#pragma warning (disable: 4244 4245 4100 4127)
#pragma warning (disable: 4634 4635)	// XML �ּ� ����
#pragma warning (disable: 4389)			// Visual Studio 2022 (vc 143) , signed, unsigned ����ġ
#pragma warning (disable: 26439 26498 26451 26444 26451 26812 4018)	// VC++ Code Guidelines warning. POCO ���̺귯���� ����...
#pragma warning (disable: 26495 6385 6386 26800)
// ���⿡ �̸� �������Ϸ��� ��� �߰�
#include <iostream>
#include <shared_mutex>
#include <thread>
#include <functional>
#include <format>
#include <chrono>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <mimalloc/mimalloc.h>
#include <memory>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mimalloc\\mimalloc.dll.lib")

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#pragma comment(lib, "Poco\\PocoDatamd.lib")
#pragma comment(lib, "Poco\\PocoDataODBCmd.lib")
#pragma comment(lib, "Poco\\PocoFoundationmd.lib")
#include<modycamel/concurrentqueue.h>
#include"RefSingleton.h"
#include"ObjectPool.h"
#include "LogManager.h"
#include "PocoTimeUtil.h"
#include "LogMacro.h"
#include "PacketChunk.h"
#include "ThreadLocal.h"
#include "IocpHandler.h"
#endif //PCH_H