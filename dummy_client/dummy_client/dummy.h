#pragma once
#include<vector>
#include<string>
#include<atomic>
void InitializeNetwork();
void GetMsgCloud(std::vector<std::string>& msgVec);
void ShutdownNetwork();
extern int64_t global_delay;
extern std::atomic_int active_clients;
