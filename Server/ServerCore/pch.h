#pragma once

// pch.h: �̸� �����ϵ� ��� �����Դϴ�.
// �Ʒ� ������ ������ �� ���� �����ϵǾ�����, ���� ���忡 ���� ���� ������ ����մϴ�.
// �ڵ� ������ �� ���� �ڵ� �˻� ����� �����Ͽ� IntelliSense ���ɿ��� ������ ��Ĩ�ϴ�.
// �׷��� ���⿡ ������ ������ ���� �� ������Ʈ�Ǵ� ��� ��� �ٽ� �����ϵ˴ϴ�.
// ���⿡ ���� ������Ʈ�� ������ �߰����� ������. �׷��� ������ ���ϵ˴ϴ�.

#ifndef PCH_H
#define PCH_H

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
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

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mimalloc.dll.lib")
#include"ObjectPool.h"
#endif //PCH_H