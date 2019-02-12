#pragma once

#include "time.h"

#define SCHEDULE_ITEMS_NUM 10

class CScheduleItem
{
public:
	enum EEffectType
	{
		EF_NONE,
		EF_SUNRISE,
		EF_RED
	};

	unsigned char id;
	bool isEnabled;
	time_t execTime;
	uint8_t effectType;
	uint8_t folderID;
	uint8_t songID;
	int lightEnabledTime;
	int soundEnabledTime;
	uint8_t dayOfWeekMask = 0;

};

#pragma pack(push,1)
class CBoardConfig
{
protected:
	CBoardConfig();
public:
	~CBoardConfig();
	static CBoardConfig Inst;

	CScheduleItem Schedule[SCHEDULE_ITEMS_NUM];

	bool UpdateScheduleItem(const CScheduleItem& item);
};
#pragma pack(pop)
