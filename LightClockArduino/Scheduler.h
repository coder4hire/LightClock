#pragma once

#define SCHEDULE_ITEMS_NUM 10
#include "time.h"
#include "ScheduleItem.h"
#include "AlarmEffect.h"

class CScheduler
{
public:
	static CScheduler Inst;
	~CScheduler();

	void CheckScheduledEvents();

	CScheduleItem Schedule[SCHEDULE_ITEMS_NUM];

	bool UpdateScheduleItem(const CScheduleItem& item);

	bool EnableScheduleItem(uint8_t index, bool isEnabled);

	void OnTimeTick();
	void RunEffectNow(CScheduleItem::EEffectType effect,int maxLength=600, bool enableRandomMusic=false);
	void StopEffects();

	bool IsCurrentEffectRunning() { return currectEffect.IsRunning(); }

protected:
	CScheduler();
	CAlarmEffect currectEffect;
	int currentEffectScheduleIdx;
	void StoreItemToEEPROM(int index);
	void LoadItemsFromEEPROM();
};

