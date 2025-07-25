#pragma once
// 포인터 사용하지 않는 싱글톤

template <class T>
class RefSingleton
{
public:
	static T& GetInstance()
	{
		static T instance;
		return instance;
	}

	virtual void Initialize() {};	// 초기화 후 사용하도록

protected:
	static T mInstance;
};

template <class T>
T RefSingleton<T>::mInstance;
