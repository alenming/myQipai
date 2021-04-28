#ifndef LOCAL_TIME_H__
#define LOCAL_TIME_H__
#include <vector>
#include <time.h>

class HourListener
{

public:

	//服务器时间每小时通知一次
	virtual void hourTick(int hour) = 0;
};

class TimeStampListener
{

public:

	TimeStampListener()
		: mTimeStamp(0)
	{
	};

	void setTimeStamp(int timeStamp){ mTimeStamp = timeStamp; };
	int getTimeStamp(){ return mTimeStamp; };

	//超时通知
	virtual void timeOut() = 0;

private:

	int mTimeStamp;
};

/**********************************需要刷新秒数显示的监听器**************************/

class TimeLocalListener
{

public:

	TimeLocalListener()
		: mTimerStamp(0)
	{
	};

	void setEndTimeStamp(int timeStamp){ mTimerStamp = timeStamp; };
	int getEndTimeStamp(){ return (int)mTimerStamp; };

	//计时通知 param: 时间间隔
	virtual void timeTick(int dt) = 0;

private:

	//结束时间戳
	time_t mTimerStamp;
};
/**********************************需要刷新秒数显示的监听器**************************/

class LocalTimer
{

public:

	LocalTimer();
	~LocalTimer();

	static LocalTimer* getInstance();

	static void destroyInstance();

public:

	//获取服务器当前时间
	time_t getCurServerTime();

	//启动时间定时器
	void startTimer(time_t serverTime);

	//暂停定时器
	void stopTimer();

	//是否到达某时间
	bool isTimeOut(int hour);

	//监听器是否存在
	bool isHourListenerExist(HourListener* listener);

	//添加监听器
	void addHourListener(HourListener* listener);

	//删除监听器
	void removeHourListener(HourListener* listener);

	//监听器是否存在
	bool isTimeStampListenerExist(TimeStampListener* listener);

	//添加监听器
	void addTimeStampListener(TimeStampListener* listener);

	//删除监听器
	void removeTimeStampListener(TimeStampListener* listener);

	//监听器是否存在
	bool isLocalTimerListenerExist(TimeLocalListener* listener);

	//添加监听器
	void addLocalTimerListener(TimeLocalListener* listener);

	//删除监听器
	void removeLocalTimerListener(TimeLocalListener* listener);

private:

	void updateTime(float dt);

private:

	static LocalTimer* mInstance;

	//小时监听器
	std::vector<HourListener*> mVectorHourListener;

	//时间戳监听器
	std::vector<TimeStampListener*> mVectorTimeStampListener;

	//本地倒计时监听器
	std::vector<TimeLocalListener*> mVectorLocalTimeListener;

	//登录时记录的服务器时间
	time_t mLoginServerTime;

	//登录时记录的本地时间
	time_t mLoginLocalTime;

	//上次记录的服务器时间
	time_t mLastRecordTime;

	//服务器时间
	time_t mCurServerTime;

};

#endif //LOCAL_TIME_H__