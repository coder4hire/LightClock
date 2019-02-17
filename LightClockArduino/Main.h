#pragma once

#include "Arduino.h"
#include "RTClib.h"
#include "IRControl.h"
#include "Scheduler.h"
#include "PlayerCtrl.h"

#define DFPLAYER_RECONNECT_INTERVAL 500
#define TRACE(x) Serial.println(x);

class CMain
{
public:
	~CMain();
	static CMain Inst;
	RTC_DS1307 RTC;
	CPlayerCtrl Player;

	void Setup();
	void Loop();

protected:
	unsigned char br;
	unsigned char c;

	CMain();
		
	void OnButtonPressed();
	void OnIRButtonPressed(IR_ACTIONS actionButton);

	bool CheckButtonStatus();
	uint8_t prevButtonState;
};

