#ifndef _MY_JSON_H__
#define _MY_JSON_H__
#include <string>
#include "json/json.h"

#include "core/KxCore.h"

#ifndef RunningInServer
#include "cocos2d.h"
#endif
/*
1:在jsoncpp库的基础上封装了一层自己习惯的api

*/

class MyJson
{
public:
	MyJson();
	MyJson(Json::Value&);
	MyJson(const char * data);
	virtual ~MyJson();
public:
	int    getInt(std::string key);
	float  getFloat(std::string key);
	double getDouble(std::string key);
	bool   getBool(std::string key);
	std::string getString(std::string key);
	MyJson getValue(std::string key);
	MyJson getValue(int key);

	void writeInt(std::string key, int value);
	void writeDouble(std::string key, double value);
	void writeBool(std::string key, bool value);
	void writeFloat(std::string key, float value);
	void writeString(std::string key, std::string value);
	void writeValue(std::string key, MyJson& value);
	void writeValueFor(std::string key, MyJson& value);

	bool removeMember(std::string key);

	std::string getString();

	size_t getSize()
	{
		return mValue.size();
	}

	MyJson at(size_t size)
	{
		if ((mValue.size()>0) && (size < mValue.size()))
		{
			return MyJson(mValue.asValue(size));
		}
		return nullptr;
	}
	
	void operator=(MyJson& my)
	{
		this->mValue = my.getValue();
	}

	virtual Json::Value getValue(void) const
	{
		return mValue;
	}
protected: 
	Json::Value mValue;

private:
	bool init();

	Json::Reader mReader;
	Json::FastWriter mWriter;
	const char * mData;
	bool isParse;
};



#endif // _MY_JSON_H__
