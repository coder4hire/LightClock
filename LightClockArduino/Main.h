#pragma once

#include <SoftwareSerial.h>
#include "DFRobotDFPlayerMini.h"

#define DFPLAYER_RECONNECT_INTERVAL 500
#define TRACE(x) Serial.println(x);

enum PINS
{
	PIN_RED = 5,
	PIN_GREEN = 6,
	PIN_BLUE = 9,
	PIN_WHITE = 10
};

class CMain
{
public:
	CMain();
	~CMain();

	void Setup();
	void Loop();

protected:
	SoftwareSerial softwareSerialPort;
	SoftwareSerial BTSerial;
	DFRobotDFPlayerMini dfPlayer;

	unsigned char c1;
	unsigned char c2;
};

