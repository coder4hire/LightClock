#pragma once
class CScheduleItem
{
public:
	enum EEffectType
	{
		EF_NONE,
		EF_SUNRISE,
		EF_DARK_RED,
		EF_RED,
		EF_DARK_GREEN,
		EF_GREEN,
		EF_DARK_BLUE,
		EF_BLUE
	};

	uint8_t ID;
	bool IsEnabled;
	int32_t ExecTime;
	uint8_t EffectType;
	uint8_t FolderID;
	uint8_t SongID;
	int32_t LightEnabledTime;
	int32_t SoundEnabledTime;
	uint8_t DayOfWeekMask;
};