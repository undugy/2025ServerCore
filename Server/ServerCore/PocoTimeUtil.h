#pragma once

#ifndef POCO_STATIC
#	define POCO_STATIC 1
#endif
#pragma warning (push)

#include <Poco/Data/Time.h>
#pragma warning (pop)

constexpr int64_t INTTIME_YEAR = 10000000000;
constexpr int64_t INTTIME_MONTH = 100000000;
constexpr int64_t INTTIME_DAY = 1000000;
constexpr int64_t INTTIME_HOUR = 10000;
constexpr int64_t INTTIME_MINUTE = 100;
constexpr int64_t INTTIME_SECOND = 1;

class PocoTimeUtil
{
public:
	static bool IsZeroTime(const Poco::DateTime& time);
	static Poco::DateTime GetZeroTime();

	static Poco::DateTime GetLocalTime();
	static Poco::DateTime MakeTime(const int& year, const int& month, const int& day, const int& hour = 0,
		const int& minute = 0, const int& second = 0);

	static int GetLocalTimeZone();
	static int64_t GetTick64();
	static int64_t PocoTimeToTick(const Poco::DateTime& time);
	static Poco::DateTime TickToPocoTime(const int64_t& tick);

};

