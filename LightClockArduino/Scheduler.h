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
	void TestRunEffect(CScheduleItem::EEffectType effect);
	void StopEffects();

protected:
	CScheduler();
	CAlarmEffect currectEffect;
	int currentEffectScheduleIdx;
	void StoreItemToEEPROM(int index);
	void LoadItemsFromEEPROM();
};

