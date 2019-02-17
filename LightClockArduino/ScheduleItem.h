#pragma once
class CScheduleItem
{
public:
	enum EEffectType
	{
		EF_NONE,
		EF_SUNRISE,
		EF_RED
	};

	uint8_t ID;
	bool IsEnabled;
	time_t ExecTime;
	uint8_t EffectType;
	uint8_t FolderID;
	uint8_t SongID;
	int32_t LightEnabledTime;
	int32_t SoundEnabledTime;
	uint8_t DayOfWeekMask = 0;
};