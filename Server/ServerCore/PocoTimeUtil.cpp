#include "pch.h"
#include "PocoTimeUtil.h"
#include <Poco/Timezone.h>
#include <chrono>
bool PocoTimeUtil::IsZeroTime(const Poco::DateTime& time)
{
	return time == GetZeroTime();
}

Poco::DateTime PocoTimeUtil::GetZeroTime()
{
	return Poco::DateTime(1900, 1, 1);
}

Poco::DateTime PocoTimeUtil::GetLocalTime()
{
	Poco::DateTime time;
	time.makeLocal(Poco::Timezone::tzd());
	return time;
}

Poco::DateTime PocoTimeUtil::MakeTime(const int& year, const int& month, const int& day, const int& hour, const int& minute, const int& second)
{
    if (year < 0
        || year > 9999
        || month <= 0
        || month > 12
        || day <= 0
        || day > 31
        || hour < 0
        || hour >= 24
        || minute < 0
        || minute >= 60
        || second < 0
        || second >= 60)
        return Poco::DateTime();

    return Poco::DateTime(year, month, day, hour, minute, second);
}

int PocoTimeUtil::GetLocalTimeZone()
{
    return Poco::Timezone::tzd() / 3600;
}

int64_t PocoTimeUtil::GetTick64()
{
    auto now = std::chrono::system_clock::now();
    return now.time_since_epoch().count();
}

int64_t PocoTimeUtil::PocoTimeToTick(const Poco::DateTime& time)
{
    return time.timestamp().epochMicroseconds() / 1000;
}

Poco::DateTime PocoTimeUtil::TickToPocoTime(const int64_t& tick)
{
    Poco::Timestamp timestamp(static_cast<Poco::Timestamp::TimeVal>(tick * 1000));
    Poco::DateTime dateTime(timestamp);
    return dateTime;
}

