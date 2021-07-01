#include "MyJson.h"


MyJson::MyJson():
isParse(false),
mData(nullptr)
{
	mValue = Json::Value();
	mWriter = Json::FastWriter();
}
MyJson::MyJson(const char * data)
{
	isParse = false;
	mData = data;
	mReader =  Json::Reader();
	mValue  =  Json::Value();

	init();
}

MyJson::MyJson(Json::Value& value)
{
	mValue = value;
}

MyJson::~MyJson()
{
	isParse = false;
	mData = nullptr;
	mValue.clear();
}

bool MyJson::init()
{
	if (!isParse)
	{
		isParse = mReader.parse(mData, mValue);
	}
	return isParse;
}

int MyJson::getInt(std::string key)
{
	return mValue[key].asInt();
}

float MyJson::getFloat(std::string key)
{
	return (float)mValue[key].asDouble();
}

double MyJson::getDouble(std::string key)
{
	return mValue[key].asDouble();
}

bool MyJson::getBool(std::string key)
{
	return mValue[key].asBool();
}

std::string MyJson::getString(std::string key)
{
	return mValue[key].asString();
}

MyJson MyJson::getValue(std::string key)
{
	return  MyJson(mValue.asValue(key.c_str()));
}

//取数组下标
MyJson MyJson::getValue(int key)
{
	return MyJson(mValue.asValue(key));
}

void MyJson::writeInt(std::string key, int value)
{
	mValue[key] = value;
}

void MyJson::writeDouble(std::string key, double value)
{
	mValue[key] = value;
}

void MyJson::writeBool(std::string key, bool value)
{
	mValue[key] = value;
}

void MyJson::writeFloat(std::string key, float value)
{
	mValue[key] = value;
}

void MyJson::writeString(std::string key, std::string value)
{
	mValue[key] = value;
}

void MyJson::writeValue(std::string key, MyJson& value)
{
	mValue[key] = value.mValue;
}

//为循环插入数组使用
void MyJson::writeValueFor(std::string key, MyJson& value)
{
	mValue[key].append(value.mValue);
}

std::string MyJson::getString()
{
	return mWriter.write(mValue);
}

bool MyJson::removeMember(std::string key)
{
	return mValue.removeMember(key) != Json::Value::null;
}




