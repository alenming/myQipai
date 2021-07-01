#ifndef __CONF_GAME_SETTING_H__
#define __CONF_GAME_SETTING_H__

#include "ConfManager.h"

/////////////////////////////////Åä±íÊý¾Ý/////////////////////////////////////////

struct  NewPlayerItem
{
	int						configId;
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
