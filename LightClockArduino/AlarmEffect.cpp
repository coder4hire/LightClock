#include "AlarmEffect.h"
#include "Main.h"
#include "RGBControl.h"
#include "BoardConfig.h"

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
		// Special case for infinite light (stopping by button press), no need to check for stop time or sound
		if (effectType == CScheduleItem::EF_INFINITE_LIGHT && isLightOn)
		{
			CRGBControl::Inst.SetRGBW(RGBW(255, 255, 64, 255));
			return;
		}

		// Other effects processing
		if (activationTime + prerollTime >= now && activationTime + prerollTime <= now + 3 
			&& !isSoundOn && maxSongLength>0)
		{
			isSoundOn = true;
			if (folderID == 255)
			{
				Serial.println("RandomPlay");
				CMain::Inst.Player.PlayRandom();
			}
			else
			{
				if (songID <= 0)
				{
					songID = 1;
				}
				CMain::Inst.Player.PlaySong(folderID+1, songID);
			}
		}

		if (isLightOn)
		{
			switch (effectType)
			{
			case CScheduleItem::EF_SUNRISE:
				SunriseTimeTick(now);
				break;
			case CScheduleItem::EF_DARK_RED:
				CRGBControl::Inst.SetRGBW(RGBW(1,0,0,0));
				break;
			case CScheduleItem::EF_DARK_GREEN:
				CRGBControl::Inst.SetRGBW(RGBW(0, 1, 0, 0));
				break;
			case CScheduleItem::EF_DARK_BLUE:
				CRGBControl::Inst.SetRGBW(RGBW(0, 0, 1, 0));
				break;
			case CScheduleItem::EF_RED:
				CRGBControl::Inst.SetRGBW(RGBW(255, 0, 0, 0));
				break;
			case CScheduleItem::EF_GREEN:
				CRGBControl::Inst.SetRGBW(RGBW(0, 255, 0, 0));
				break;
			case CScheduleItem::EF_BLUE:
				CRGBControl::Inst.SetRGBW(RGBW(0, 0, 255, 0));
				break;

			}
		}

		if (now >= activationTime + prerollTime + maxSongLength && isSoundOn)
		{
			CMain::Inst.Player.Stop();
			isSoundOn = false;
		}

		if (((now >= activationTime + prerollTime + maxLightLength) ||
			(CBoardConfig::Inst.DoesBackligthDisableRGB && CMain::Inst.GetBackSensorReadings()>CBoardConfig::Inst.LightThresholdBack))
			&& isLightOn )
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
	isSoundOn = false;

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

	if (maxSongLength > 0 || isSoundOn)
	{
		CMain::Inst.Player.Stop();
	}
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
		color.R = 200l * diff / phaseLen;
	}
	else if (diff < 2 * phaseLen)
	{
		color.R = 190l + 65l * (diff - phaseLen) / phaseLen;
		color.G = 208l * (diff - phaseLen) / phaseLen;
	}
	else if (diff < phaseLen*3)
	{
		color.R = 255;
		color.G = 208;
		color.B = 128l * (diff - 2 * phaseLen) / phaseLen;
		color.W = 255l * (diff - 2 * phaseLen) / phaseLen;
	}
	else
	{
		color.R = 255;
		color.G = 208;
		color.B = 128;
		color.W = 255;
	}
	CRGBControl::Inst.SetRGBW(color);
}