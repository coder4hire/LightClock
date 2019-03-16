#include "BoardConfig.h"
#include "Scheduler.h"
#include <EEPROM.h>

CBoardConfig CBoardConfig::Inst;

CBoardConfig::CBoardConfig()
{
	LoadFromEEPROM();

	if (Volume > 255)
	{
		Volume = 255;
	}

	if (LightThresholdBack > 1023)
	{
		LightThresholdBack = 1000;
	}
}


CBoardConfig::~CBoardConfig()
{
}

void CBoardConfig::StoreToEEPROM()
{
	int offset = sizeof(CScheduleItem)*SCHEDULE_ITEMS_NUM;
	eeprom_update_block((CBoardConfigItems*)this, (void *)offset, sizeof(CBoardConfigItems));
}

void CBoardConfig::LoadFromEEPROM()
{
	int offset = sizeof(CScheduleItem)*SCHEDULE_ITEMS_NUM;
	eeprom_read_block((CBoardConfigItems*)this, (void *)offset, sizeof(CBoardConfigItems));
}

