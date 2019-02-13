#pragma once

#include "NeoSWSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "RTClib.h"
#include "IRControl.h"
#include "Scheduler.h"

#define DFPLAYER_RECONNECT_INTERVAL 500
#define TRACE(x) Serial.println(x);

class CMain
{
public:
	~CMain();
	static CMain Inst;
	RTC_DS1307 RTC;

	void Setup();
	void Loop();

protected:
	unsigned char br;
	unsigned char c;

	CMain();

	NeoSWSerial softwareSerialPort;
	DFRobotDFPlayerMini dfPlayer;
		
	void OnButtonPressed();
	void OnIRButtonPressed(IR_ACTIONS actionButton);

	bool CheckButtonStatus();
};

