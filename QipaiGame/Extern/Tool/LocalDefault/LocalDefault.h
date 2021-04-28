#ifndef __LOCAL_DEFAULT_H__
#define __LOCAL_DEFAULT_H__

#include "CommonHead.h"

#include "XML/XML.h"

#ifdef RunningInServer
#include "Common/Data_.h"
#else
#include "cocos2d.h"
#endif

#define GetIntFromXML		LocalDefault::getInstance()->getIntegerForKeyValue
#define SetIntToXML			LocalDefault::getInstance()->setIntegerForKeyValue
#define GetStringFromXML	LocalDefault::getInstance()->getStringForKeyValue
#define SetStringToXML		LocalDefault::getInstance()->setStringForKeyValue
#define GetBoolFromXML		LocalDefault::getInstance()->getBoolForKeyValue
#define SetBoolToXML		LocalDefault::getInstance()->setBoolForKeyValue
#define GetFloatFromXML		LocalDefault::getInstance()->getFloatForKeyValue
#define SetFloatToXML		LocalDefault::getInstance()->setFloatForKeyValue
#define GetDoubleFromXML	LocalDefault::getInstance()->getDoubleForKeyValue
#define SetDoubleToXML		LocalDefault::getInstance()->setDoubleForKeyValue
#define GetDataFromXML		LocalDefault::getInstance()->getDataForKeyValue
#define SetDataToXML		LocalDefault::getInstance()->setDataForKeyValue

class  LocalDefault
{
public:
	LocalDefault();
	~LocalDefault();
	static LocalDefault* getInstance();
	static void destroyInstance();
public:
	enum ENCRYPT_TYPE  //�Ƿ����
	{
		IS_EN,
		NO_EN
	};
public:
	bool	getBoolForKeyValue(const std::string nKey);											//��boolֵ
	int		getIntegerForKeyValue(const std::string nKey);										//��intֵ
	float	getFloatForKeyValue(const std::string nKey);											//��floatֵ
	double	getDoubleForKeyValue(const std::string nKey);											//��floatֵ
	std::string	getStringForKeyValue(const std::string nKey);                                        //��stringֵ


#ifdef RunningInServer
	Data_	getDataForKeyValue(const char* pKey);
#else
	cocos2d::Data	getDataForKeyValue(const char* pKey);
#endif
	void	setBoolForKeyValue(const std::string nKey, bool value);								//дboolֵ
	void	setIntegerForKeyValue(const std::string nKey, int value);							//дintֵ
	void	setFloatForKeyValue(const std::string nKey, float value);							//дfloatֵ
	void	setDoubleForKeyValue(const std::string nKey, double value);							//дdoubleֵ
	void	setStringForKeyValue(const std::string nKey, const std::string& value);				//дstringֵ

#ifdef RunningInServer
	void	setDataForKeyValue(const char* pKey, const Data_& value) ;
#else
	void	setDataForKeyValue(const char* pKey, const cocos2d::Data& value) ;
#endif
	virtual ENCRYPT_TYPE getEncryption(void) const { return _mIsEn; }
	virtual void setEncryption(ENCRYPT_TYPE var){ _mIsEn = var; }

protected:
	ENCRYPT_TYPE _mIsEn;

private:

	const std::string& getXMLFilePath();
	bool isXMLFileExist();
    void initXMLFilePath();
	bool init();

	std::string	EncryptData(const char* szPass) const;		//�ַ�������
	std::string	CrevasseData(const char* szEncryPass) const;

	void setValueForKey(const char* pKey, const char* pValue);
	std::string getValueForKey(const char* pKey);

    static LocalDefault* _userDefault;
    static std::string _filePath;
    static bool _isFilePathInitialized;
	XML* _mXML;

};




#endif // __LOCAL_DEFAULT_H__
