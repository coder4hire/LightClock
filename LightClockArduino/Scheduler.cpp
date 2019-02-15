#include "Scheduler.h"

CScheduler CScheduler::Inst;

CScheduler::CScheduler()
{
	for (int i = 0; i < SCHEDULE_ITEMS_NUM; i++)
	{
		Schedule[i].id = i;
	}
}

CScheduler::~CScheduler()
{
}

void CScheduler::CheckScheduledEvents()
{
}

bool CScheduler::UpdateScheduleItem(const CScheduleItem & item)
{
	for (int i = 0; i < SCHEDULE_ITEMS_NUM; i++)
	{
		if (Schedule[i].id == item.id)
		{
			Schedule[i] = item;
			return true;
		}
	}
	return false;
}

