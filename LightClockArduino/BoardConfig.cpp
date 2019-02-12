#include "BoardConfig.h"

CBoardConfig CBoardConfig::Inst;

CBoardConfig::CBoardConfig()
{
}


CBoardConfig::~CBoardConfig()
{
}

bool CBoardConfig::UpdateScheduleItem(const CScheduleItem & item)
{
	for (int i = 0; i < SCHEDULE_ITEMS_NUM; i++)
	{
		if (Schedule[i].id == item.id)
		{
			Schedule[i].id = item.id;
			return true;
		}
	}
	return false;
}
