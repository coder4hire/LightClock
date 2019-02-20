#include "Scheduler.h"
#include <time.h>
#include "Main.h"
#include <EEPROM.h>

CScheduler CScheduler::Inst;

CScheduler::CScheduler()
{
	LoadItemsFromEEPROM();

	// Check for integrity
	for (int i = 0; i < SCHEDULE_ITEMS_NUM; i++)
	{
		if (Schedule[i].ID != i)
		{
			Schedule[i].ID = i;
			StoreItemToEEPROM(i);
		}
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
			Schedule[i] = item;
			StoreItemToEEPROM(i);
			return true;
		}
	}
	return false;
}

bool CScheduler::EnableScheduleItem(uint8_t index, bool isEnabled)
{
	if (index < SCHEDULE_ITEMS_NUM && Schedule[index].ID == index)
	{
		Schedule[index].IsEnabled = isEnabled;
		StoreItemToEEPROM(index);
	}
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

void CScheduler::StoreItemToEEPROM(int index)
{
	if (index < SCHEDULE_ITEMS_NUM)
	{
		int offset = sizeof(CScheduleItem)*index;
		uint8_t* pItem = (uint8_t*)(Schedule+offset);
		eeprom_update_block(pItem, (void *)offset, sizeof(CScheduleItem));
	}
}

void CScheduler::LoadItemsFromEEPROM()
{
	eeprom_read_block(Schedule, (void *)0, SCHEDULE_ITEMS_NUM*sizeof(CScheduleItem));
}

