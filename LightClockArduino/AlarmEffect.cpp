#include "AlarmEffect.h"
#include "Main.h"
#include "RGBControl.h"


CAlarmEffect::CAlarmEffect()
{
	activationTime = 0;
	activationMillis = 0;

	songID = 0;
	folderID = 0;
	maxLightLength = 0;
	maxSongLength = 0;
	effectType = CScheduleItem::EF_NONE;
}

CAlarmEffect::~CAlarmEffect()
{
}

void CAlarmEffect::OnTimeTick()
{
	time_t prerollTime = GetPrerollTime(effectType);
	time_t now = CMain::Inst.RTC.now().unixtime();
	if (activationTime != 0)
	{
		if (activationTime + prerollTime >= now && activationTime + prerollTime <= now + 3 && !isSoundOn)
		{
			isSoundOn = true;
			if (folderID == 255)
			{
				CMain::Inst.Player.PlayRandom();
			}
			else
			{
				CMain::Inst.Player.PlaySong(folderID, songID);
			}
		}

		if (isLightOn)
		{
			switch (effectType)
			{
			case CScheduleItem::EF_SUNRISE:
				SunriseTimeTick(now);
				break;
			}
		}

		if (now >= activationTime + prerollTime + maxSongLength && isSoundOn)
		{
			CMain::Inst.Player.Stop();
			isSoundOn = false;
		}

		if (now >= activationTime + prerollTime + maxLightLength && isLightOn)
		{
			isLightOn = false;
			CRGBControl::Inst.SetRGBW(0);
		}
	}
}

time_t CAlarmEffect::GetPrerollTime(CScheduleItem::EEffectType effectType)
{
	switch (effectType)
	{
	case CScheduleItem::EF_SUNRISE:
		return 60;
	}
	return 0;
}

void CAlarmEffect::Start(CScheduleItem item)
{
	Stop();
	activationTime = CMain::Inst.RTC.now().unixtime();
	activationMillis = millis();
	isLightOn = true;

	songID = item.SongID;
	folderID = item.FolderID;
	maxLightLength = item.LightEnabledTime;
	maxSongLength = item.SoundEnabledTime;
	effectType = (CScheduleItem::EEffectType) item.EffectType;
}

void CAlarmEffect::Stop()
{
	activationTime = 0;
	activationMillis = 0;
	isLightOn = false;
	isSoundOn = false;
	CRGBControl::Inst.SetRGBW(0);
	CMain::Inst.Player.Stop();
}

void CAlarmEffect::SunriseTimeTick(time_t now)
{
	unsigned long phaseLen = GetPrerollTime(effectType)*1000 / 3;	// Using millis

	RGBW color(0);
	unsigned long diff = millis() - activationMillis;
	if (diff > 0xFFFFFF) // Handling overflow
	{
		diff += 0xFFFFFFFF;
	}

	if (diff < phaseLen)
	{
		color.R = 200 * diff / phaseLen;
	}
	else if (diff < 2 * phaseLen)
	{
		color.R = 200 + 55 * (diff - phaseLen) / phaseLen;
		color.G = 255 * (diff - phaseLen) / phaseLen;
	}
	else if (diff < phaseLen*3)
	{
		color.R = 255;
		color.G = 255;
		color.B = 160 * (diff - 2 * phaseLen) / phaseLen;
		color.W = 255 * (diff - 2 * phaseLen) / phaseLen;
	}
	else
	{
		color.R = 255;
		color.G = 255;
		color.B = 160;
		color.W = 255;
	}
	CRGBControl::Inst.SetRGBW(color);
}
