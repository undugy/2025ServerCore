// ServerApp.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include <iostream>
#include "pch.h"
class A
{
public:
    int a = 0;
};
int main()
{
    std::cout << "Hello World!\n";
    auto t = ObjectPool<A>::GetInstance().Acquire(1);

    int a;
    std::cin >> a;
}
