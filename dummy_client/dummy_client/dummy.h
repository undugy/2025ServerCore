#pragma once
#include<vector>
#include<string>
#include<atomic>
void InitializeNetwork();
void GetPointCloud(int* size, float** points);
void GetMsgCloud(std::vector<std::string>& msgVec);
void ShutdownNetwork();
extern int global_delay;
extern std::atomic_int active_clients;