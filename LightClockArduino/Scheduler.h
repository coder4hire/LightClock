#pragma once

#define SCHEDULE_ITEMS_NUM 10
#include "time.h"

class CScheduleItem
{
public:
	enum EEffectType
	{
		EF_NONE,
		EF_SUNRISE,
		EF_RED
	};

	uint8_t id;
	bool isEnabled;
	time_t execTime;
	uint8_t effectType;
	uint8_t folderID;
	uint8_t songID;
	int32_t lightEnabledTime;
	int32_t soundEnabledTime;
	uint8_t dayOfWeekMask = 0;
};

class CScheduler
{
public:
	static CScheduler Inst;
	~CScheduler();

	void CheckScheduledEvents();

	CScheduleItem Schedule[SCHEDULE_ITEMS_NUM];

	bool UpdateScheduleItem(const CScheduleItem& item);

protected:
	CScheduler();
};

