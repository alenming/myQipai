#include "ConfGameSetting.h"
#include "LogManager.h"

#define CHECK_RETURN(success) if(!(success)) { LOG("Execute False On File %s Line %d : %s", __FILE__, __LINE__, #success); return false; };

bool ConfNewPlayerSetting::LoadCSV(const std::string& str)
{
	CsvLoader csvLoader;
	if(!loadCsv(csvLoader, str.c_str(), 3))
		return false;
	while (csvLoader.NextLine())
	{
		std::vector<int> temp;
		NewPlayerItem *item = new NewPlayerItem;
		item->configId = csvLoader.NextInt();
		item->HeadId = csvLoader.NextInt();
		item->UserLv = csvLoader.NextInt();
		item->UserExp = csvLoader.NextInt();
		item->Gold = csvLoader.NextInt();
		item->Diamond = csvLoader.NextInt();

		m_Datas[item->configId] = item;
	}
	return true;
}