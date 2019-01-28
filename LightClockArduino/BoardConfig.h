#pragma once

#include "time.h"

#define ALARMS_NUM 10

class CAlarmConfig
{
public:
	enum EEffectType
	{
		EF_NONE,
		EF_SUNRISE,
		EF_RED
	};

	time_t execTime;
	uint8_t effectType;
	long effectDuration;
	uint8_t songNum;
	int8_t folderNum;
	long songDuration;
};

#pragma pack(push,1)
class CBoardConfig
{
public:
	CBoardConfig();
	~CBoardConfig();

	CAlarmConfig Alarms[ALARMS_NUM];
};
#pragma pack(pop)
