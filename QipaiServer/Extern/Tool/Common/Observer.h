#ifndef _DATASOURCE_H_
#define _DATASOURCE_H_

#include <vector>
using namespace std;

class Observable;

/************************************************************************/
/* 监听数据类*/
/************************************************************************/
struct ObserverParam
{
	ObserverParam()
		: self(nullptr)
		, updateData(nullptr)
		, id(-1)
	{
	}

	//自身
	Observable* self;
	//更新数据
	void* updateData;
	//类型ID
	int id;
};

/************************************************************************/
/* Observer类，数据侦听器*/
/************************************************************************/
class Observer
{

public:
	Observer(){}
	~Observer(){}

public:

	//数据变化后，通知UI更新数据
	virtual void updateSelf(void* data) = 0;
};

/************************************************************************/
/* Observable类，Observer通知中心，通知所有注册的observer*/
/************************************************************************/
class Observable
{ 
public:
	Observable(){}
	~Observable()
	{
		observers.clear();
	}

public:

	//判断是否相等
	bool equals(const Observable& obser);

	//添加观察者
	void addObserver(Observer* a);

	//移除观察者
	void removeObserver(Observer* target);

	//通知观察者
	void notifyObservers(void* data = NULL);

	//判断观察者是否存在
	bool isObserverExist(Observer* target);

private:

	//observers
	vector<Observer*> observers;

};
#endif