#include "Observer.h"

bool Observable::equals(const Observable& obser)
{
	if(&observers == &obser.observers)
		return true;
	else
		return false;
}

void Observable::addObserver(Observer* a)
{
	if(!isObserverExist(a))
		observers.push_back(a);
}

void Observable::notifyObservers(void* data) 
{ 
	for (vector<Observer*>::iterator iter=observers.begin();iter!=observers.end();iter++)
	{
		if(data == nullptr)
		{
			ObserverParam param;
			param.self = this;
			(*iter)->updateSelf(&param); 
		}
		else
		{
			ObserverParam* pParam = (ObserverParam*)data;
			pParam->self = this;
			(*iter)->updateSelf(pParam); 
		}
	}
} 

bool Observable::isObserverExist(Observer* target)
{
	for (size_t i = 0 ; i < observers.size() ; i++)
	{
		Observer* observer =observers.at(i);
		if ( !observer )
		{
			continue;
		}
		if ( observer == target )
		{
			return true;
		}
	}
	return false;
}

void Observable::removeObserver(Observer* target)
{
	if(observers.size()==0)return;
	vector<Observer*>::iterator it = observers.begin();
	while( it != observers.end() )
	{
		if ( *it == NULL )
		{
			continue;
		}
		if ( *it == target )
		{
			observers.erase(it);
			return;
		}
		++it;
	}
}
