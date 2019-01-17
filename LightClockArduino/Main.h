#pragma once

#include "NeoSWSerial.h"
#include "DFRobotDFPlayerMini.h"

#define DFPLAYER_RECONNECT_INTERVAL 500
#define TRACE(x) Serial.println(x);

#define CMD_MAX_SIZE 128

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
	~CMain();
	static CMain Inst;

	void Setup();
	void Loop();

protected:
	CMain();

	NeoSWSerial softwareSerialPort;
	DFRobotDFPlayerMini dfPlayer;

	NeoSWSerial BTSerial;
	unsigned char btCmdBuffer[CMD_MAX_SIZE];
	unsigned char rcvdCmd[CMD_MAX_SIZE];
	unsigned short btCmdBufLength;
	
	static void handleBTChar(uint8_t c);
	void OnBTCharReceived(uint8_t c);
	bool IsBTCommandComplete();
	bool ReadBTCommand();

	bool CheckButtonStatus();

	unsigned char c1;
	unsigned char c2;
};

