#pragma once
#include "RefSingleton.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <spdlog/common.h>
#include <concurrent_priority_queue.h>

enum class LogType
{
	Info,
	Warning,
	Error
};

struct LogStruct
{
	LogStruct() = default;
	LogStruct(LogType lv, int64_t time, std::string_view log, bool write) : lv(lv), time(time), log(log), write(write) {}

	LogType lv = LogType::Info;
	int64_t time = 0ll;
	std::string log = "";
	bool write = false;
	constexpr bool operator < (const LogStruct& left) const
	{
		return (time > left.time);
	}
};

/// <summary>
/// spdlog 매핑 클래스
/// </summary>
class LogManager : public RefSingleton<LogManager>
{
public:
	LogManager();
	~LogManager();

public:
	void Initialize(const std::string& loggerName = "basic_logger", const std::string& filepath = "../logs/");
	void Launch();

	template <class... Args>
	void Log(const int64_t& time, const LogType& type, const bool& view, const bool& write, const std::string& fmt, Args&&... args);

private:
	void Write(const int64_t& time, const LogType& type, const std::string_view& log);
	void View(const int64_t& time, const LogType& type, const std::string_view& log);
	void Log(const int64_t& time, const LogType& type, const std::string_view& log, spdlog::logger* logger = nullptr);

private:	
	std::shared_ptr<spdlog::logger> _logger = nullptr;
	concurrency::concurrent_priority_queue<LogStruct> _logs;
	std::chrono::seconds _offset = {};
	std::jthread _threads;
};

template <class... Args>
void LogManager::Log(const int64_t& time, const LogType& type, const bool& view, const bool& write, const std::string& fmt, Args&&... args)
{
	std::string log = std::vformat(fmt, std::make_format_args(args...));


	_logs.push({ type, time, log, write});

}
