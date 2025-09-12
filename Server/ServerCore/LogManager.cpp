#include "pch.h"
#include "LogManager.h"
#include "ThreadManager.h"
#include <fileSystem>
LogManager::LogManager()
{

}

LogManager::~LogManager()
{
	LogStruct log;
	while (_logs.try_pop(log) == false)
	{
		View(log.time, log.lv, log.log);
		if (log.write)
		{
			Write(log.time, log.lv, log.log);
		}
	}
}

void LogManager::Initialize(const std::string& loggerName /*= "basic_logger"*/, const std::string& filepath /*= "../logs/"*/)
{
	try
	{
		Poco::DateTime now = PocoTimeUtil::GetLocalTime();
		std::string formattedDate = Poco::DateTimeFormatter::format(now, "%Y-%m-%d") + ".txt";
		std::filesystem::create_directories(filepath);
		_logger = spdlog::basic_logger_mt(loggerName, filepath + formattedDate);

		auto localTime = std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now());
		_offset = localTime.get_info().offset;
		std::cout << "Log init success" << std::endl;
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		std::cout << "Log init failed: " << ex.what() << std::endl;
	}
}

void LogManager::Launch()
{

	ThreadManager::GetInstance().PushThread([&]() {
		while (true)
		{
			LogStruct log;
			while (_logs.try_pop(log) == false)
			{
				if(log.log.empty())
					continue;
				View(log.time, log.lv, log.log);
				if (log.write)
				{
					Write(log.time, log.lv, log.log);
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}});

}

void LogManager::Write(const int64_t& time, const LogType& type, const std::string_view& log)
{
	Log(time, type, log, _logger.get());
}

void LogManager::View(const int64_t& time, const LogType& type, const std::string_view& log)
{
	Log(time, type, log);
}

void LogManager::Log(const int64_t& time, const LogType& type, const std::string_view& log, spdlog::logger* logger)
{
	if (logger == nullptr)
		logger = spdlog::default_logger_raw();

	spdlog::level::level_enum lvl = spdlog::level::off;
	switch (type)
	{
	case LogType::Info:
		lvl = spdlog::level::info;
		break;
	case LogType::Warning:
		lvl = spdlog::level::warn;
		break;
	case LogType::Error:
		lvl = spdlog::level::err;
		break;
	default:
		break;
	}

	std::chrono::milliseconds duration(time);
	duration -= _offset;
	std::chrono::system_clock::time_point time_point(duration);
	logger->log(time_point, spdlog::source_loc{}, lvl, log);
	if (lvl == spdlog::level::err)
		logger->flush();
}
