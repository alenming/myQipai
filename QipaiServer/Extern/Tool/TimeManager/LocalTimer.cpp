#include "LocalTimer.h"
#include <algorithm>

LocalTimer* LocalTimer::mInstance = nullptr;

LocalTimer::LocalTimer()
	: mLoginServerTime(0)
	, mLoginLocalTime(0)
	, mLastRecordTime(0)
	, mCurServerTime(0)
{
}

LocalTimer::~LocalTimer()
{
	stopTimer();
}

LocalTimer* LocalTimer::getInstance()
{
	if(!mInstance)
	{
		mInstance = new LocalTimer();
	}
	return mInstance;
}

void LocalTimer::destroyInstance()
{
	if(mInstance)
	{
		delete mInstance;
		mInstance = nullptr;
	}
}

time_t LocalTimer::getCurServerTime()
{
	return mCurServerTime;
}

void LocalTimer::startTimer(time_t serverTime)
{
	mLoginServerTime = serverTime;
	mLoginLocalTime = time(nullptr);
	mCurServerTime = mLastRecordTime = serverTime;

	//Director::getInstance()->getScheduler()->schedule(
	//	CC_CALLBACK_1(LocalTimer::updateTime, this), this, 1, false, "LocalTimer::startTimer");
}

void LocalTimer::stopTimer()
{
	//Director::getInstance()->getScheduler()->unschedule("LocalTimer::startTimer", this);
}

bool LocalTimer::isTimeOut(int hour)
{
	tm* curServerTime = localtime(&mCurServerTime);
	return curServerTime->tm_hour >= hour;
}

void LocalTimer::updateTime( float dt )
{
	time_t currentTime = time(nullptr);

	//记录上一次的时间
	mLastRecordTime = mCurServerTime;

	//计算当前服务器时间
	mCurServerTime = mLoginServerTime + currentTime - mLoginLocalTime;

	tm formatCurTime = { 0 };
	tm formatLastTime = { 0 };

	formatCurTime = *(localtime(&mCurServerTime));
	formatLastTime = *(localtime(&mLastRecordTime));

	//判断小时数是否发生变化, 如果发生变化，说明跨小时了
	if (formatCurTime.tm_hour != formatLastTime.tm_hour)
	{
		for (auto listener : mVectorHourListener)
		{
			if (listener)
			{
				listener->hourTick(formatCurTime.tm_hour);
			}
		}
	}

	if(!mVectorLocalTimeListener.empty())
	{
		//移除超时的监听器，并调用监听器的计时接口
		mVectorLocalTimeListener.erase(
			std::remove_if(mVectorLocalTimeListener.begin(), mVectorLocalTimeListener.end(), 
			[this](TimeLocalListener* listener)
		{
			if (listener)
			{
				if(mCurServerTime <= listener->getEndTimeStamp())
				{
					int dt = (int)(mCurServerTime - mLastRecordTime);
					listener->timeTick(dt);	
					return false;
				}
				return true;
			}
			return false;
		}	
		), mVectorLocalTimeListener.end());
	}
	

	if (!mVectorTimeStampListener.empty())
	{
		//移除超时的监听器，并调用监听器的超时接口
		mVectorTimeStampListener.erase(
			std::remove_if(mVectorTimeStampListener.begin(), mVectorTimeStampListener.end(), 
			[this](TimeStampListener* listener)
			{
				if (listener && mCurServerTime >= listener->getTimeStamp())
				{
					listener->timeOut();
					return true;
				}
				return false;
			}	
		), mVectorTimeStampListener.end());
	}
}

bool LocalTimer::isHourListenerExist(HourListener* listener)
{
	for (auto tempListener : mVectorHourListener)
	{
		if (!tempListener)
		{
			continue;
		}

		if (tempListener == listener)
		{
			return true;
		}
	}
	return false;
}

void LocalTimer::addHourListener(HourListener* listener)
{
	if(!isHourListenerExist(listener))
		mVectorHourListener.push_back(listener);
}

void LocalTimer::removeHourListener(HourListener* listener)
{
	if(mVectorHourListener.empty()) return;
	std::vector<HourListener*>::iterator it = mVectorHourListener.begin();
	while(it != mVectorHourListener.end())
	{
		if (*it == nullptr )
		{
			continue;
		}

		if (*it == listener)
		{
			mVectorHourListener.erase(it);
			return;
		}
		++it;
	}
}

bool LocalTimer::isTimeStampListenerExist( TimeStampListener* listener )
{
	for (auto tempListener : mVectorTimeStampListener)
	{
		if (!tempListener)
		{
			continue;
		}

		if (tempListener == listener)
		{
			return true;
		}
	}
	return false;
}

void LocalTimer::addTimeStampListener( TimeStampListener* listener )
{
	if(!isTimeStampListenerExist(listener))
		mVectorTimeStampListener.push_back(listener);
}

void LocalTimer::removeTimeStampListener( TimeStampListener* listener )
{
	if(mVectorTimeStampListener.empty()) return;
	std::vector<TimeStampListener*>::iterator it = mVectorTimeStampListener.begin();
	while(it != mVectorTimeStampListener.end())
	{
		if (*it == nullptr )
		{
			continue;
		}

		if (*it == listener)
		{
			mVectorTimeStampListener.erase(it);
			return;
		}
		++it;
	}
}

bool LocalTimer::isLocalTimerListenerExist( TimeLocalListener* listener )
{
	for (auto tempListener : mVectorLocalTimeListener)
	{
		if (!tempListener)
		{
			continue;
		}

		if (tempListener == listener)
		{
			return true;
		}
	}
	return false;
}

void LocalTimer::addLocalTimerListener( TimeLocalListener* listener )
{
	if(!isLocalTimerListenerExist(listener))
		mVectorLocalTimeListener.push_back(listener);
}

void LocalTimer::removeLocalTimerListener( TimeLocalListener* listener )
{
	if(mVectorLocalTimeListener.empty()) return;
	std::vector<TimeLocalListener*>::iterator it = mVectorLocalTimeListener.begin();
	while(it != mVectorLocalTimeListener.end())
	{
		if (*it == nullptr )
		{
			continue;
		}

		if (*it == listener)
		{
			mVectorLocalTimeListener.erase(it);
			return;
		}
		++it;
	}
}
