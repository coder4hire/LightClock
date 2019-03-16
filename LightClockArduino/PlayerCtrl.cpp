#include "PlayerCtrl.h"

CPlayerCtrl::CPlayerCtrl():
	softwareSerialPort(3, 4) // RX, TX
{
}


CPlayerCtrl::~CPlayerCtrl()
{
}

void CPlayerCtrl::Init()
{
	softwareSerialPort.begin(9600);
	//dfPlayer.setTimeOut(1000);
	int retries = 10;
	ListenPort();
	while (!dfPlayer.begin(softwareSerialPort, true, retries == 10) && retries--)
	{
		Serial.println(F("Unable to connect to dfPlayer"));
	}
	dfPlayer.stop();
}

void CPlayerCtrl::PlayRandom()
{
	ListenPort();
	dfPlayer.randomAll();
	dfPlayer.start();
}

void CPlayerCtrl::PlaySong(uint8_t folderID, uint8_t songID)
{
	ListenPort();
	dfPlayer.playFolder(folderID, songID);
}

void CPlayerCtrl::Play()
{
	ListenPort();
	dfPlayer.play();
}

void CPlayerCtrl::Stop()
{
	ListenPort();
	dfPlayer.stop();
}

void CPlayerCtrl::Next()
{
	ListenPort();
	dfPlayer.next();
}

void CPlayerCtrl::Previous()
{
	ListenPort();
	dfPlayer.previous();
}

void CPlayerCtrl::VolumeUp()
{
	ListenPort();
	dfPlayer.volumeUp();
}

void CPlayerCtrl::VolumeDown()
{
	ListenPort();
	dfPlayer.volumeDown();
}

void CPlayerCtrl::SetVolume(uint8_t vol)
{
	ListenPort();
	dfPlayer.volume(vol);
}

void CPlayerCtrl::ListenPort()
{
	if (!softwareSerialPort.isListening())
	{
		softwareSerialPort.listen();
	}
}
