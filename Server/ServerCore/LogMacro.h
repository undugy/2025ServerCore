#pragma once
#define VIEW_INFO(fmt, ...)					LogManager::GetInstance().Log(PocoTimeUtil::GetTick64(), LogType::Info, true, false, fmt, __VA_ARGS__)
#define WRITE_INFO(fmt, ...)				LogManager::GetInstance().Log(PocoTimeUtil::GetTick64(), LogType::Info, false, true, fmt, __VA_ARGS__)
#define VIEW_WRITE_INFO(fmt, ...)			LogManager::GetInstance().Log(PocoTimeUtil::GetTick64(), LogType::Info, true, true, fmt, __VA_ARGS__)

#define VIEW_WARNING(fmt, ...)				LogManager::GetInstance().Log(PocoTimeUtil::GetTick64(), LogType::Warning, true, false, fmt, __VA_ARGS__)
#define WRITE_WARNING(fmt, ...)				LogManager::GetInstance().Log(PocoTimeUtil::GetTick64(), LogType::Warning, false, true, fmt, __VA_ARGS__)
#define VIEW_WRITE_WARNING(fmt, ...)		LogManager::GetInstance().Log(PocoTimeUtil::GetTick64(), LogType::Warning, true, true, fmt, __VA_ARGS__)

#define VIEW_ERROR(fmt, ...)				LogManager::GetInstance().Log(PocoTimeUtil::GetTick64(), LogType::Error, true, false, fmt, __VA_ARGS__)
#define WRITE_ERROR(fmt, ...)				LogManager::GetInstance().Log(PocoTimeUtil::GetTick64(), LogType::Error, false, true, fmt, __VA_ARGS__)
#define VIEW_WRITE_ERROR(fmt, ...)			LogManager::GetInstance().Log(PocoTimeUtil::GetTick64(), LogType::Error, true, true, fmt, __VA_ARGS__)
