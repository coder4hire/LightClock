#include "Scheduler.h"
#include <time.h>
#include "Main.h"

CScheduler CScheduler::Inst;

CScheduler::CScheduler()
{
	for (int i = 0; i < SCHEDULE_ITEMS_NUM; i++)
	{
		Schedule[i].ID = i;
	}

	currentEffectScheduleIdx = -1;
}

CScheduler::~CScheduler()
{
}

void CScheduler::CheckScheduledEvents()
{
}

bool CScheduler::UpdateScheduleItem(const CScheduleItem & item)
{
	Serial.println("Item coming:");
	Serial.println(item.ID);
	for (int i = 0; i < SCHEDULE_ITEMS_NUM; i++)
	{
		if (Schedule[i].ID == item.ID)
		{
			Serial.println("Got item!!!");
			Serial.println(item.ExecTime);
			Schedule[i] = item;
			return true;
		}
	}
	return false;
}

void CScheduler::OnTimeTick()
{
	time_t now = CMain::Inst.RTC.now().unixtime();
	for (int i = 0; i < SCHEDULE_ITEMS_NUM; i++)
	{
		const CScheduleItem* pItem = Schedule + i;
		time_t prerollTime = CAlarmEffect::GetPrerollTime((CScheduleItem::EEffectType) pItem->EffectType);
		time_t futurePoint = now + prerollTime;
		tm* pParsedFuture = gmtime(&futurePoint);
		time_t futureTimeNoDate = pParsedFuture->tm_hour * 3600l + pParsedFuture->tm_min * 60;

		// Checking if item is to be executed
		if ((1 << pParsedFuture->tm_wday) & pItem->DayOfWeekMask
			&& futureTimeNoDate>=pItem->ExecTime && futureTimeNoDate <= pItem->ExecTime + prerollTime + 3 &&
			(currentEffectScheduleIdx!=i || !currectEffect.IsRunning()))
		{
			currentEffectScheduleIdx = i;
			currectEffect.Start(*pItem);
		}
	}

	currectEffect.OnTimeTick();
}

void CScheduler::TestRunEffect(CScheduleItem::EEffectType effect)
{
	CScheduleItem testItem;
	testItem.EffectType = (uint8_t)effect;
	testItem.FolderID = 255;
	testItem.SongID = 0;
	testItem.LightEnabledTime = 600;
	testItem.SoundEnabledTime = 600;

	currentEffectScheduleIdx = -1;
	currectEffect.Start(testItem);
}

void CScheduler::StopEffects()
{
	currectEffect.Stop();
}

