#include "LocalDefault.h"
#include "StringFormat/StringFormat.h"

#ifdef RunningInServer
#include "LocalDefault/base64_.h"
#endif



#define ENCRYPT_KEY_LENGTH 1
#define LOCAL_DEFAULT    "LocalDefault"
#define XML_FILE_NAME    "LocalDefault.xml"
#define DATA "Data"
std::string default_data = "<?xml version=\"1.0\" encoding=\"utf-8\" ?> <LocalDefault> <Data  /> </LocalDefault>";

LocalDefault* LocalDefault::_userDefault = nullptr;
std::string LocalDefault::_filePath = std::string("");
bool LocalDefault::_isFilePathInitialized = false;

LocalDefault::LocalDefault()
:_mIsEn(ENCRYPT_TYPE::IS_EN)
{
	_mXML = new XML();
}

LocalDefault::~LocalDefault()
{
	delete _mXML;
}

LocalDefault* LocalDefault::getInstance()
{
	if (!_userDefault)
	{
		_userDefault = new LocalDefault();
		_userDefault->init();
	}

	return _userDefault;
}

void LocalDefault::destroyInstance()
{
	do 
	{ 
		delete _userDefault; 
		_userDefault = nullptr; 
	} while (0);
}

void LocalDefault::setIntegerForKeyValue(const std::string nKey, int value)
{
	char szValue[128];
	memset(szValue, 0, 128);
	sprintf(szValue, "%d", value);
	std::string Key = EncryptData(nKey.c_str());
	std::string Value = EncryptData(szValue);
	setValueForKey(Key.c_str(), Value.c_str());
}

//存储
void LocalDefault::setBoolForKeyValue(const std::string nKey, bool value)
{
	std::string Key = EncryptData(nKey.c_str());
	std::string svalue = EncryptData(value ? "1" : "0");
	setValueForKey(Key.c_str(), svalue.c_str());
}

void LocalDefault::setStringForKeyValue(const std::string nKey, const std::string & sValue)
{
	std::string Key = EncryptData(nKey.c_str());
	std::string Value = EncryptData(sValue.c_str());
	setValueForKey(Key.c_str(), Value.c_str());
}

void LocalDefault::setFloatForKeyValue(const std::string nKey, float value)
{
	char szValue[128];
	memset(szValue, 0, 128);
	sprintf(szValue, "%f", value);
	std::string Key = EncryptData(nKey.c_str());
	std::string Value = EncryptData(szValue);
	setValueForKey(Key.c_str(), Value.c_str());
}

void LocalDefault::setDoubleForKeyValue(const std::string nKey, double value)
{
	char szValue[128];
	memset(szValue, 0, 128);
	//保留九点小数
	sprintf(szValue, "%.9f", value);
	std::string Key = EncryptData(nKey.c_str());
	std::string Value = EncryptData(szValue);
	setValueForKey(Key.c_str(), Value.c_str());
}
#ifdef RunningInServer
void LocalDefault::setDataForKeyValue(const char* nKey, const Data_& value)
{
	if (!nKey || strlen(nKey) <= 0)
	{
		return;
	}

	char *encodedData = 0;

	base64Encode(value.getBytes(), static_cast<unsigned int>(value.getSize()), &encodedData);
	std::string Key = EncryptData(nKey);
	setValueForKey(Key.c_str(), encodedData);

	if (encodedData)
	{
		free(encodedData);
	}
}
#else
void LocalDefault::setDataForKeyValue(const char* nKey, const cocos2d::Data& value)
{
	if (!nKey || strlen(nKey) <= 0)
	{
		return;
	}

	char *encodedData = 0;

	cocos2d::base64Encode(value.getBytes(), static_cast<unsigned int>(value.getSize()), &encodedData);
	std::string Key = EncryptData(nKey);
	setValueForKey(Key.c_str(), encodedData);

	if (encodedData)
	{
		free(encodedData);
	}
}
#endif

//读取
int LocalDefault::getIntegerForKeyValue(const std::string nKey)
{
	std::string Key = EncryptData(nKey.c_str());

	std::string s = getValueForKey(Key.c_str());
	std::string parseKey = CrevasseData(s.c_str());
	return (parseKey.length() != 0 ? atoi(parseKey.c_str()) : 0);
}

bool LocalDefault::getBoolForKeyValue(const std::string nKey)
{
	std::string Key = EncryptData(nKey.c_str());

	std::string s = getValueForKey(Key.c_str());
	std::string parseKey = CrevasseData(s.c_str());
	bool isBool = parseKey == "1" ? true : false;
	return (parseKey.length() != 0 ? isBool : false);
}

std::string LocalDefault::getStringForKeyValue(const std::string nKey)
{
	std::string Key = EncryptData(nKey.c_str());
	std::string s = getValueForKey(Key.c_str());
	std::string parseKey = CrevasseData(s.c_str());
	return (parseKey.length() != 0 ? parseKey : "");
}

float LocalDefault::getFloatForKeyValue(const std::string nKey)
{
	std::string Key = EncryptData(nKey.c_str());
	std::string s = getValueForKey(Key.c_str());
	std::string parseKey = CrevasseData(s.c_str());
	return (parseKey.length() != 0 ? StringFormat::toNum<float>(parseKey) : 0);
}

double LocalDefault::getDoubleForKeyValue(const std::string nKey)
{
	std::string Key = EncryptData(nKey.c_str());
	std::string s = getValueForKey(Key.c_str());
	std::string parseKey = CrevasseData(s.c_str());
	return (parseKey.length() != 0 ? StringFormat::toNum<double>(parseKey) : 0);
}

#ifdef RunningInServer
Data_ LocalDefault::getDataForKeyValue(const char* nKey)
{
	std::string Key = EncryptData(nKey);
	std::string result = getValueForKey(Key.c_str());

	Data_ ret = Data_::Null;

	if (result.c_str())
	{
		unsigned char * decodedData = nullptr;
		int decodedDataLen = base64Decode((unsigned char*)result.c_str(), (unsigned int)strlen(result.c_str()), &decodedData);
		if (decodedData) {
			ret.fastSet(decodedData, decodedDataLen);
		}
	}
	return ret;
}
#else
cocos2d::Data LocalDefault::getDataForKeyValue(const char* nKey)
{
	std::string Key = EncryptData(nKey);
	std::string result = getValueForKey(Key.c_str());

	cocos2d::Data ret = cocos2d::Data::Null;

	if (result.c_str())
	{
		unsigned char * decodedData = nullptr;
		int decodedDataLen = cocos2d::base64Decode((unsigned char*)result.c_str(), (unsigned int)strlen(result.c_str()), &decodedData);
		if (decodedData) {
			ret.fastSet(decodedData, decodedDataLen);
		}
	}
	return ret;
}
#endif


std::string LocalDefault::getValueForKey(const char* pKey)
{
	std::string result = "";

	if (!pKey || strlen(pKey) == 0)
	{
		return result;
	}

	if (_mXML->Load(_filePath.c_str()))
	{
		if (!_mXML->FindElem(LOCAL_DEFAULT))
		{
			return result;
		}
		_mXML->IntoElem();
		if (_mXML->FindElem(DATA))
		{
			result = _mXML->GetAttrib(pKey);
		}
		_mXML->Save(_filePath.c_str());
		return result;
	}
	return result;
}


void LocalDefault::setValueForKey(const char* pKey, const char* pValue)
{

	if (!pKey || strlen(pKey) == 0)
	{
		return;
	}

	if (_mXML->Load(_filePath.c_str()))
	{
		if (!_mXML->FindElem(LOCAL_DEFAULT))
		{
			return;
		}
		_mXML->IntoElem();
		if (_mXML->FindElem(DATA))
		{
			if (_mXML->GetAttrib(pKey) == "")
			{
				_mXML->AddAttrib(pKey, pValue);
			}
			else
			{
				_mXML->SetAttrib(pKey, pValue);
			}
		}
		_mXML->Save(_filePath.c_str());
	}
}

//生成密文
std::string LocalDefault::EncryptData(const char* szPass) const
{
	if (_mIsEn == ENCRYPT_TYPE::NO_EN)
	{
		return std::string(szPass);
	}
	//生成密钥
	unsigned char szRandKey[ENCRYPT_KEY_LENGTH];
	srand((unsigned)time(NULL));
	szRandKey[0] = strlen(szPass);
	for (int i = 1; i < sizeof(szRandKey); i++)
	{
		szRandKey[i] = rand() % 255;
	}
	//生成密文
	unsigned char bTempCode = 0;
	char szBuffer[8];
	std::string strEncryptPass;
	const unsigned int uKeyLength = sizeof(szRandKey) / sizeof(szRandKey[0]);
	int nTimes = (szRandKey[0] + uKeyLength - 1) / uKeyLength*uKeyLength;
	for (int i = 0; i < nTimes; i++)
	{
		if (i < szRandKey[0]) bTempCode = szPass[i] ^ szRandKey[i%uKeyLength];
		else bTempCode = szRandKey[i%uKeyLength] ^ (rand() % 255);
		sprintf(szBuffer, "%02x%02x", szRandKey[i%uKeyLength], bTempCode);
		strEncryptPass += szBuffer;
	}

	return strEncryptPass;
}

//解开密文
std::string LocalDefault::CrevasseData(const char* szEncryPass) const
{
	if (_mIsEn == ENCRYPT_TYPE::NO_EN)
	{
		return std::string(szEncryPass);
	}
	if ((szEncryPass != NULL) && (strlen(szEncryPass) >= ENCRYPT_KEY_LENGTH * 4))
	{
		//效验长度
		char * pEnd = NULL;
		int nSoureLength = 0;
		char szSoureLengthBuf[3] = { szEncryPass[0], szEncryPass[1], 0 };
		nSoureLength = strtol(szSoureLengthBuf, &pEnd, 16);
		if (strlen(szEncryPass) != ((nSoureLength + ENCRYPT_KEY_LENGTH - 1) / ENCRYPT_KEY_LENGTH*ENCRYPT_KEY_LENGTH * 4))
			return "";

		//解开密码
		std::string strCrevassePass;
		for (int i = 0; i < nSoureLength; i++)
		{
			unsigned char szKeyBuffer[3] = { (unsigned char)szEncryPass[i * 4], (unsigned char)szEncryPass[i * 4 + 1], 0 };
			char szEncryptBuffer[3] = { szEncryPass[i * 4 + 2], szEncryPass[i * 4 + 3], 0 };
			unsigned int uKey, uEncrypt;
			uKey = strtol((char *)szKeyBuffer, &pEnd, 16);
			uEncrypt = strtol(szEncryptBuffer, &pEnd, 16);
			strCrevassePass += (char)((unsigned char)uKey ^ (unsigned char)uEncrypt);
		}
		return strCrevassePass;
	}
	return "";
}

bool LocalDefault::init()
{
	_userDefault->initXMLFilePath();
	_userDefault->isXMLFileExist();
	return true;
}

bool LocalDefault::isXMLFileExist()
{
    FILE *fp = fopen(_filePath.c_str(), "rb");
	if (fp)
	{
		fseek(fp, 0L, SEEK_END);
		unsigned int nFileLen = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		if (nFileLen < strlen(default_data.c_str()))
		{
			fclose(fp);
			fp = fopen(_filePath.c_str(), "w");
			if (fp)
			{
				fwrite(default_data.c_str(), strlen(default_data.c_str()), 1, fp);
				fclose(fp);
				return true;
			}
		}
		fclose(fp);
		return true;
	}
	else
	{
		fp = fopen(_filePath.c_str(), "w");
		if (fp)
		{
			fwrite(default_data.c_str(), strlen(default_data.c_str()), 1, fp);
			fclose(fp);
			return true;
		}
	}
	return true;
}

void LocalDefault::initXMLFilePath()
{
    if (! _isFilePathInitialized)
    {
#ifdef RunningInServer
		_filePath +=  XML_FILE_NAME;
#else
		_filePath += cocos2d::FileUtils::getInstance()->getWritablePath() + XML_FILE_NAME;
#endif
        _isFilePathInitialized = true;
    }    
}

const std::string& LocalDefault::getXMLFilePath()
{
    return _filePath;
}



