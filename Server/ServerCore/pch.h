#pragma once

// pch.h: �̸� �����ϵ� ��� �����Դϴ�.
// �Ʒ� ������ ������ �� ���� �����ϵǾ�����, ���� ���忡 ���� ���� ������ ����մϴ�.
// �ڵ� ������ �� ���� �ڵ� �˻� ����� �����Ͽ� IntelliSense ���ɿ��� ������ ��Ĩ�ϴ�.
// �׷��� ���⿡ ������ ������ ���� �� ������Ʈ�Ǵ� ��� ��� �ٽ� �����ϵ˴ϴ�.
// ���⿡ ���� ������Ʈ�� ������ �߰����� ������. �׷��� ������ ���ϵ˴ϴ�.

#ifndef PCH_H
#define PCH_H
#define VERSION_RIO

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#pragma warning(disable : 26498 4819 6386 26800)	  // SPD Log
#pragma warning (disable: 26439 26498 26451 26444 26451 26812)	// VC++ Code Guidelines warning. POCO ���̺귯���� ����...
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