#pragma once
#include <time.h>
#include "ScheduleItem.h"

class CAlarmEffect
{
public:
	CAlarmEffect();
	virtual ~CAlarmEffect();

	virtual void OnTimeTick();
	static time_t GetPrerollTime(CScheduleItem::EEffectType effectType);

	bool IsRunning() { return isLightOn || isSoundOn; }
	void Start(CScheduleItem item);
	void Stop();
	
protected:	
	time_t activationTime;
	unsigned long activationMillis;
	time_t maxLightLength;
	time_t maxSongLength;

	bool isLightOn;
	bool isSoundOn;
	uint8_t songID;
	uint8_t folderID;

	void SunriseTimeTick(time_t now);

	CScheduleItem::EEffectType effectType;
};

