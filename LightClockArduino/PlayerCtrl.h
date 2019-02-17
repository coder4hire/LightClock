#pragma once
#include "DFRobotDFPlayerMini.h"
#include "NeoSWSerial.h"

class CPlayerCtrl
{
public:
	CPlayerCtrl();
	~CPlayerCtrl();

	void Init();
	void PlayRandom();
	void PlaySong(uint8_t folderID, uint8_t songID);
	void Stop();
	void Next();
	void Previous();
	void SetVolume(uint8_t vol);

private:
	void ListenPort();

	NeoSWSerial softwareSerialPort;
	DFRobotDFPlayerMini dfPlayer;

};

