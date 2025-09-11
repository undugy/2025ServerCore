#pragma once
// ������ ������� �ʴ� �̱���

template <class T>
class RefSingleton
{
public:
	static T& GetInstance()
	{
		static T instance;
		return instance;
	}

	virtual void Initialize() {};	// �ʱ�ȭ �� ����ϵ���

};

