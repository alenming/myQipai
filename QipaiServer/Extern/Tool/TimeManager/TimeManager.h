#ifndef _TIME_MANAGER_H__
#define _TIME_MANAGER_H__
#include <string>

struct DayTime
{
	int hour;
	int minutes;
	int second;
};

struct MonthTime
{
	int month;
	int day;
	DayTime dayTime;
};

struct WeekTime
{
	int weekDay;
	DayTime dayTime;
};

struct YearTime
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
};

struct MyTimeVal {
	long    tv_sec;         /* seconds */
	long    tv_usec;        /* and microseconds */
};


#define MINSECONDS 60
#define HOURSECONDS 3600
// һ�������
#define DAYSECONDS  86400
#define WEEKSECONDS  604800

#define WEEKDAY 7

class TimeManager
{
public:
	//��ȡ��ǰʱ���	
	static long getNowTimeStamp();

	// ��ȡ�������ڼ� ����ֵΪ1 - 7
	static int getWeekday();
	// ����ָ��ʱ���ȡ���ܼ� ����ֵΪ1 - 7
	static int getWeekByTime(time_t now);
	// ��ȡ��ǰ�����ַ���20170220
	static std::string getCurrDate();
	// ��������ʱ���ʽ��Ϊ00:00:00��ʽ
	static std::string formatTime(int seconds, char separator = ':');
	//��ȡʱ��ṹ��
	static struct tm* getTimeStruct();
	//��ѯʱ���������ʱ���Ƿ���ָ����ʱ��
	static bool isDayTimeOver(time_t prev, DayTime &dayTime);
	//��ѯʱ���������ʱ���Ƿ���ָ������ʱ��
	static bool isWeekTimeOver(time_t prev, WeekTime &weekTime);
	//��ѯʱ���������ʱ���Ƿ���ָ����ʱ��
	static bool isMonthTimeOver(time_t prev, MonthTime &month);
	//��ѯʱ���������ʱ���Ƿ����second��
	static bool isSecondOver(time_t prev, int second);
	//��ѯ�����Ƿ�Ϊָ����ʱ����
	static bool isDayTime(DayTime &beginTime, DayTime &endTime);
	//��ѯ�����Ƿ���ָ��������
	static bool isYearTime(YearTime &beginYearTime, YearTime &endYearTime);
	//��ѯ�Ƿ񵽵�, �����ؼ���
	static int overDay(time_t prev, DayTime &dayTime);
	//��ѯ�Ƿ�ָ����������, �����ؼ���
	static int overWeek(time_t prev, WeekTime &weekTime);
	//��ѯ�Ƿ�ָ������, �����ؼ���
	static int overSecond(time_t prev, int second);
    // �����ϴλָ���ʱ�������ǰʱ�䡢ʱ�����������м侭���˶��ٴμ��
    static int getTimesBySecond(time_t lastTime, time_t now, int interval);
    // �����ϴλָ���ʱ�������ǰʱ�䡢�����м侭���˶�����
    static int getTimesByDay(time_t lastTime, time_t now);
    // �����ϴλָ���ʱ�������ǰʱ�䡢�����м侭���˶�����
    static int getTimesByWeek(time_t lastTime, time_t now);

    // ���ݵ�ǰʱ���Ŀ��ʱ�䣬����ʣ��ʱ��
    // ���㵽ָ��ʱ����ʣ��ʱ��
    static int getSecondToNextSecond(time_t now, int target);
    // ���㵽��һ��ָ��ʱ���ʣ��ʱ��
    static int getSecondToNextDay(time_t now, int hour, int min, int second);
    // ���㵽��һ��ָ��ʱ���ʣ��ʱ�䣬day��ʾ�ܼ���ֵΪ 1 - 7
    static int getSecondToNextWeek(time_t now, int day, int hour, int min, int second);
    //����ĳ��ʱ������¸���ʱ���ֵ�ʱ���
    static int getNextTimeStamp(time_t prev, int nextMin, int nextHour);
    //����ĳ��ʱ������ܼ���ʱ���ֵ�ʱ��� wDay��1~7
    static int getWNextTimeStamp(time_t prev, int nextMin, int nextHour, int wDay);
private:
	static void gettimeofday(struct MyTimeVal * val, struct timezone *);
};

#endif //_TIME_MANAGER_H__
