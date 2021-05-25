#ifndef __CONF_GAME_SETTING_H__
#define __CONF_GAME_SETTING_H__

#include "ConfManager.h"

#define NO_KEY_DATA(_DATA_TYPE_)	\
public:								\
	inline _DATA_TYPE_& getData()	\
	{								\
	return m_Data;				\
	}								\
protected:							\
	_DATA_TYPE_ m_Data;				\


/////////////////////////////////Åä±íÊý¾Ý/////////////////////////////////////////

class NewPlayerItem
{
public:
	int						HeadId;
	int						UserLv;
	int						UserExp;
	int						Gold;
	int						Diamond;
};

class ConfNewPlayerSetting : public ConfBase
{
public:
	virtual bool LoadCSV(const std::string& str);
};

#endif
