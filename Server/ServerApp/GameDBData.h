#pragma once
#include "DBHandler.h"

class spChatRequest : public DBData
{
public:
	int32_t Result = -3;
	int32_t SessionID = 0;

	std::string Name = {};
	std::string Msg = {};
};
