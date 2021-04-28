#ifndef LOCAL_TIME_H__
#define LOCAL_TIME_H__
#include <vector>
#include <time.h>

class HourListener
{

public:

	//������ʱ��ÿСʱ֪ͨһ��
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

	//��ʱ֪ͨ
	virtual void timeOut() = 0;

private:

	int mTimeStamp;
};

/**********************************��Ҫˢ��������ʾ�ļ�����**************************/

class TimeLocalListener
{

public:

	TimeLocalListener()
		: mTimerStamp(0)
	{
	};

	void setEndTimeStamp(int timeStamp){ mTimerStamp = timeStamp; };
	int getEndTimeStamp(){ return (int)mTimerStamp; };

	//��ʱ֪ͨ param: ʱ����
	virtual void timeTick(int dt) = 0;

private:

	//����ʱ���
	time_t mTimerStamp;
};
/**********************************��Ҫˢ��������ʾ�ļ�����**************************/

class LocalTimer
{

public:

	LocalTimer();
	~LocalTimer();

	static LocalTimer* getInstance();

	static void destroyInstance();

public:

	//��ȡ��������ǰʱ��
	time_t getCurServerTime();

	//����ʱ�䶨ʱ��
	void startTimer(time_t serverTime);

	//��ͣ��ʱ��
	void stopTimer();

	//�Ƿ񵽴�ĳʱ��
	bool isTimeOut(int hour);

	//�������Ƿ����
	bool isHourListenerExist(HourListener* listener);

	//��Ӽ�����
	void addHourListener(HourListener* listener);

	//ɾ��������
	void removeHourListener(HourListener* listener);

	//�������Ƿ����
	bool isTimeStampListenerExist(TimeStampListener* listener);

	//��Ӽ�����
	void addTimeStampListener(TimeStampListener* listener);

	//ɾ��������
	void removeTimeStampListener(TimeStampListener* listener);

	//�������Ƿ����
	bool isLocalTimerListenerExist(TimeLocalListener* listener);

	//��Ӽ�����
	void addLocalTimerListener(TimeLocalListener* listener);

	//ɾ��������
	void removeLocalTimerListener(TimeLocalListener* listener);

private:

	void updateTime(float dt);

private:

	static LocalTimer* mInstance;

	//Сʱ������
	std::vector<HourListener*> mVectorHourListener;

	//ʱ���������
	std::vector<TimeStampListener*> mVectorTimeStampListener;

	//���ص���ʱ������
	std::vector<TimeLocalListener*> mVectorLocalTimeListener;

	//��¼ʱ��¼�ķ�����ʱ��
	time_t mLoginServerTime;

	//��¼ʱ��¼�ı���ʱ��
	time_t mLoginLocalTime;

	//�ϴμ�¼�ķ�����ʱ��
	time_t mLastRecordTime;

	//������ʱ��
	time_t mCurServerTime;

};

#endif //LOCAL_TIME_H__