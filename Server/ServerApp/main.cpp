// ServerApp.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include <iostream>
#include "ServerPch.h"
#include "ServerApp.h"
int main()
{
	ServerApp server;
	if (server.Init() == false)
	{
		std::cout << "Init Fail" << std::endl;
		return 0;
	}

	server.Update();
	return 0;
}
