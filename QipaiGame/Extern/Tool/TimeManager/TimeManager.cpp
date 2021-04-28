#include "TimeManager.h"
#include <time.h>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <Windows.h>
#endif //

//获取当前时间戳	
long TimeManager::getNowTimeStamp()
{
	MyTimeVal nowTimeval;
	gettimeofday(&nowTimeval, NULL);
	return nowTimeval.tv_sec;
}

//今天星期几
int TimeManager::getWeekday()
{
	struct tm* tm;
	time_t timep;
	time(&timep);

	tm = localtime(&timep);
	return (tm->tm_wday == 0) ? 7 : tm->tm_wday;
}

int TimeManager::getWeekByTime(time_t now)
{
	tm* date = localtime(&now);
	return date->tm_wday == 0 ? 7 : date->tm_wday;
}

// 获取当前日期字符串YYYYMMDD (added by Phil 05/11/2015 @zcjoy)
std::string TimeManager::getCurrDate()
{
	struct tm* tm;
	time_t timep;
	time(&timep);

	tm = localtime(&timep);
	char date[32] = { 0 };
	sprintf(date, "%d%02d%02d", (int)tm->tm_year + 1900, (int)tm->tm_mon + 1, (int)tm->tm_mday);
	return date;
}

std::string TimeManager::formatTime(int seconds, char separator)
{
	if (seconds < 0)
	{
		return std::string("");
	}

	char time[16] = "";
	sprintf(time, "%02d%c%02d%c%02d", seconds / 3600, separator, (seconds % 3600) / 60, 
		separator, ((seconds % 3600) % 60) % 60);
	return std::string(time);
}

struct tm* TimeManager::getTimeStruct()
{
	struct tm* nowtime;
	time_t nowTimeval;
	time(&nowTimeval);
	nowtime = localtime(&nowTimeval);

	nowtime->tm_year += 1900;
	nowtime->tm_mon += 1;
	return nowtime;
}

void TimeManager::gettimeofday(struct MyTimeVal * val, struct timezone *)
{
#if ((MING_TARGET_PLATFORM) == (MING_PLATFORM_WIN32))
	if (val)
	{
		SYSTEMTIME wtm;
		GetLocalTime(&wtm);

		struct tm tTm;
		tTm.tm_year = wtm.wYear - 1900;
		tTm.tm_mon = wtm.wMonth - 1;
		tTm.tm_mday = wtm.wDay;
		tTm.tm_hour = wtm.wHour;
		tTm.tm_min = wtm.wMinute;
		tTm.tm_sec = wtm.wSecond;
		tTm.tm_isdst = -1;

		val->tv_sec = (long)mktime(&tTm);       // time_t is 64-bit on win32
		val->tv_usec = wtm.wMilliseconds * 1000;
	}
#else
	val->tv_sec = time(NULL);//这句返回的只是一个时间cuo
#endif
}


bool TimeManager::isDayTimeOver(time_t prev, DayTime &daytime)
{
	time_t now = time(NULL);
	tm *pPrevTm = localtime(&prev);
	tm *pNowTm = localtime(&now);

	if (now - prev >= DAYSECONDS)
	{
		//如果大于1天, 返回true
		return true;
	}
	else
	{
		if (pPrevTm->tm_mday != pNowTm->tm_mday)
		{
			//不同一天的, now跨过指定时间即可
			bool ret1 = pNowTm->tm_hour > daytime.hour;
			bool ret2 = pNowTm->tm_hour == daytime.hour && pNowTm->tm_min > daytime.minutes;
			bool ret3 = pNowTm->tm_hour == daytime.hour && pNowTm->tm_min == daytime.minutes && pNowTm->tm_sec >= daytime.second;

			return ret1 || ret2 || ret3;
		}
		else
		{
			//同一天的, prev在指定时间之前
			bool ret1 = pPrevTm->tm_hour < daytime.hour;
			bool ret2 = pPrevTm->tm_hour == daytime.hour && pPrevTm->tm_min < daytime.minutes;
			bool ret3 = pPrevTm->tm_hour == daytime.hour && pPrevTm->tm_min == daytime.minutes && pPrevTm->tm_sec < daytime.second;
			//now在指定时间之后
			bool ret4 = pNowTm->tm_hour > daytime.hour;
			bool ret5 = pNowTm->tm_hour == daytime.hour && pNowTm->tm_min > daytime.minutes;
			bool ret6 = pNowTm->tm_hour == daytime.hour && pNowTm->tm_min == daytime.minutes && pNowTm->tm_sec >= daytime.second;

			return (ret1 || ret2 || ret3) && (ret4 || ret5 || ret6);
		}
	}
	return false;
}

bool TimeManager::isWeekTimeOver(time_t prev, WeekTime &weekTime)
{
	time_t now = time(NULL);
	tm *pPrevTm = localtime(&prev);
	tm *pNowTm = localtime(&now);

	if (now - prev >= WEEKSECONDS)
	{
		//超过一个星期天
		return true;
	}
	else
	{ 
		//prev在指定星期天之前
		bool ret1 = pPrevTm->tm_wday < weekTime.weekDay;
		bool ret2 = pPrevTm->tm_wday == weekTime.weekDay
			&& pPrevTm->tm_hour < weekTime.dayTime.hour;
		bool ret3 = pPrevTm->tm_wday == weekTime.weekDay && pPrevTm->tm_hour == weekTime.dayTime.hour
			&& pPrevTm->tm_min < weekTime.dayTime.minutes;
		bool ret4 = pPrevTm->tm_wday == weekTime.weekDay && pPrevTm->tm_hour == weekTime.dayTime.hour
			&& pPrevTm->tm_min == weekTime.dayTime.minutes && pPrevTm->tm_sec < weekTime.dayTime.second;
		//now在指定星期天之后
		bool ret5 = pNowTm->tm_wday > weekTime.weekDay;
		bool ret6 = pNowTm->tm_wday == weekTime.weekDay
			&& pNowTm->tm_hour > weekTime.dayTime.hour;
		bool ret7 = pNowTm->tm_wday == weekTime.weekDay && pNowTm->tm_hour == weekTime.dayTime.hour
			&& pNowTm->tm_min > weekTime.dayTime.minutes;
		bool ret8 = pNowTm->tm_wday == weekTime.weekDay && pNowTm->tm_hour == weekTime.dayTime.hour
			&& pNowTm->tm_min == weekTime.dayTime.minutes && pNowTm->tm_sec >= weekTime.dayTime.second;

		return (ret1 || ret2 || ret3 || ret4) && (ret5 || ret6 || ret7 || ret8);
	}
	return false;
}

bool TimeManager::isMonthTimeOver(time_t prev, MonthTime &month)
{
	time_t now = time(NULL);
	return true;
}

bool TimeManager::isSecondOver(time_t prev, int second)
{
	time_t now = time(NULL);
	return now - prev >= second;
}

bool TimeManager::isDayTime(DayTime &beginTime, DayTime &endTime)
{
	time_t now = time(NULL);
	tm *pNowTm = localtime(&now);

	bool ret1 = pNowTm->tm_hour > beginTime.hour;
	bool ret2 = pNowTm->tm_hour == beginTime.hour && pNowTm->tm_min > beginTime.minutes;
	bool ret3 = pNowTm->tm_hour == beginTime.hour && pNowTm->tm_min == beginTime.minutes && pNowTm->tm_sec >= beginTime.second;

	bool ret4 = pNowTm->tm_hour < endTime.hour;
	bool ret5 = pNowTm->tm_hour == endTime.hour && pNowTm->tm_min < endTime.minutes;
	bool ret6 = pNowTm->tm_hour == endTime.hour && pNowTm->tm_min == endTime.minutes && pNowTm->tm_sec <= endTime.second;

	return (ret1 || ret2 || ret3) && (ret4 || ret5 || ret6);
}

bool TimeManager::isYearTime(YearTime &beginYearTime, YearTime &endYearTime)
{
	tm beginTm;
	tm endTm;
	memset(&beginTm, 0, sizeof(beginTm));
	memset(&endTm, 0, sizeof(endTm));

	time_t now = time(NULL);
	time_t beginTime;
	time_t endTime;

	beginTm.tm_year = beginYearTime.year;
	beginTm.tm_mon = beginYearTime.month;
	beginTm.tm_mday = beginYearTime.day;
	beginTm.tm_hour = beginYearTime.hour;
	beginTm.tm_min = beginYearTime.minute;
	beginTm.tm_sec = beginYearTime.second;
	
	endTm.tm_year = endYearTime.year;
	endTm.tm_mon = endYearTime.month;
	endTm.tm_mday = endYearTime.day;
	endTm.tm_hour = endYearTime.hour;
	endTm.tm_min = endYearTime.minute;
	endTm.tm_sec = endYearTime.second;

	beginTime = mktime(&beginTm);
	endTime = mktime(&beginTm);

	return beginTime <= now && endTime >= now;
}

int TimeManager::overDay(time_t prev, DayTime &dayTime)
{
	time_t now = time(NULL);
	tm *pPrevTm = localtime(&prev);
	tm *pNowTm = localtime(&now);
	int times = 0;
	time_t d = now - prev;
	// 先算到指定时间需要的秒数
	int seconds = 0;
	if (pPrevTm->tm_hour <= dayTime.hour)
	{
		seconds = (dayTime.hour - pPrevTm->tm_hour) * 60 * 60;
	}
	else
	{
		seconds = ((24 - pPrevTm->tm_hour) + dayTime.hour) * 60 * 60;
	}

	seconds = (seconds + dayTime.minutes * 60 + dayTime.second) - (pPrevTm->tm_min * 60 + pPrevTm->tm_sec);
	if (d >= seconds)
	{
		times += 1;
		// 剩下的满1天+1
		d -= seconds;
		times += static_cast<int>(d / DAYSECONDS);
		return times;
	}
	return 0;
}

int TimeManager::overWeek(time_t prev, WeekTime &weekTime)
{
	time_t now = time(NULL);
	tm *pPrevTm = localtime(&prev);
	tm *pNowTm = localtime(&now);
	int times = 0;
	time_t d = now - prev;

	int seconds = 0;
	if (pPrevTm->tm_wday <= weekTime.weekDay)
	{
		seconds = (weekTime.weekDay - pPrevTm->tm_wday) * DAYSECONDS;
	}
	else
	{
		seconds = ((7 - pPrevTm->tm_wday) + weekTime.weekDay) * DAYSECONDS;
	}

	seconds = (seconds + weekTime.dayTime.hour * 60 * 60 + weekTime.dayTime.minutes * 60 + weekTime.dayTime.second)
		- (pPrevTm->tm_hour * 60 * 60 + pPrevTm->tm_min * 60 + pPrevTm->tm_sec);

	if (d >= seconds)
	{
		times += 1;
		d -= seconds;
		times += static_cast<int>(d / WEEKSECONDS);
		return times;
	}
	return 0;
}

int TimeManager::overSecond(time_t prev, int second)
{
	time_t now = time(NULL);
	time_t d = now - prev;
	return static_cast<int>(d/second);
}

int TimeManager::getTimesBySecond(time_t lastTime, time_t now, int interval)
{
    return (int)((now - lastTime) / interval);
}

int TimeManager::getTimesByDay(time_t lastTime, time_t now)
{
	return (int)((now - lastTime) / DAYSECONDS);
}

int TimeManager::getTimesByWeek(time_t lastTime, time_t now)
{
	return (int)((now - lastTime) / WEEKSECONDS);
}

int TimeManager::getSecondToNextSecond(time_t lastTime, int target)
{
	return (int)(target - lastTime);
}

int TimeManager::getSecondToNextDay(time_t now, int hour, int min, int second)
{
    tm* date = localtime(&now);
    int hourOffset = hour - date->tm_hour;
    int minOffset = min - date->tm_min;
    int secondOffset = second - date->tm_sec;

    // 当前到指定时刻的偏移时间（有可能为负数）
    int secondToTime = hourOffset * HOURSECONDS + minOffset * MINSECONDS + second;
    // 如果今天未到指定的时刻，返回今日剩余秒数
    if (secondToTime > 0)
    {
        return secondToTime;
    }
    else
    {
        // 否则返回到明日指定时刻的剩余秒数
        return DAYSECONDS + secondToTime;
    }
}

int TimeManager::getSecondToNextWeek(time_t now, int day, int hour, int min, int second)
{
    tm* date = localtime(&now);
    int dayOffset = day - (date->tm_wday + 1);
    int hourOffset = hour - date->tm_hour;
    int minOffset = min - date->tm_min;
    int secondOffset = second - date->tm_sec;
    // 当前到指定时刻的偏移时间（有可能为负数）
    int secondToTime = dayOffset * DAYSECONDS + hourOffset * HOURSECONDS + minOffset * MINSECONDS + second;
    // 如果未到本周指定的时刻，返回本周剩余秒数
    if (secondToTime > 0)
    {
        return secondToTime;
    }
    else
    {
        // 否则返回到下周指定时刻的剩余秒数
        return WEEKSECONDS + secondToTime;
    }
}

int TimeManager::getNextTimeStamp(time_t prev, int nextMin, int nextHour)
{
    tm *pPrevTm = localtime(&prev);
    int n = (nextHour - pPrevTm->tm_hour) * HOURSECONDS + (nextMin - pPrevTm->tm_min) * MINSECONDS - pPrevTm->tm_sec;
    if (n <= 0)
    {
        n += DAYSECONDS + int(prev);
    }
    else
    {
        n += int(prev);
    }

    return n;
}

int TimeManager::getWNextTimeStamp(time_t prev, int nextMin, int nextHour, int wDay)
{
    wDay %= WEEKDAY; // 注：周7为0
    tm *pPrevTm = localtime(&prev);

    int w = wDay - pPrevTm->tm_wday;
    if (w < 0)
    {
        w += WEEKDAY;
    }

    int n = (nextHour - pPrevTm->tm_hour) * HOURSECONDS + (nextMin - pPrevTm->tm_min) * MINSECONDS - pPrevTm->tm_sec;
    return w * DAYSECONDS + n + (int)prev;
}