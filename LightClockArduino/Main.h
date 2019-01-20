#pragma once

#include "NeoSWSerial.h"
#include "DFRobotDFPlayerMini.h"

#define DFPLAYER_RECONNECT_INTERVAL 500
#define TRACE(x) Serial.println(x);

#define CMD_MAX_SIZE 128

class CMain
{
public:
	~CMain();
	static CMain Inst;

	void Setup();
	void Loop();

protected:
	unsigned char br;
	unsigned char c;
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
};

