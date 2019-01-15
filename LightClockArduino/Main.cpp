#include "Main.h"

#include "DFRobotDFPlayerMini.h"

CMain CMain::Inst;

CMain::CMain():
	softwareSerialPort(3, 4), // RX, TX
	BTSerial(PIN_A3,2) // RX | TX
{
	c1 = 0;
	c2 = 0;
	btCmdBufLength = 0;
}


CMain::~CMain()
{
}

void CMain::Setup()
{
	Serial.begin(115200);
	BTSerial.begin(9600);
	softwareSerialPort.begin(9600);
	
	dfPlayer.setTimeOut(1000);
	int retries = 10;
	while (!dfPlayer.begin(softwareSerialPort,true,retries==10) && retries--)
	{  
		TRACE(F("Unable to connect to DFPlayer"));
	}


	dfPlayer.volume(2);
	dfPlayer.play();
	Serial.print("Vol: ");
	Serial.println(dfPlayer.readVolume());
	softwareSerialPort.listen();
	Serial.print("Vol: ");
	Serial.println(dfPlayer.readVolume());
	Serial.print("Track: ");
	Serial.println(dfPlayer.readCurrentFileNumber());

	//--- Pins initialization
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_GREEN, OUTPUT);
	pinMode(PIN_BLUE, OUTPUT);
	pinMode(PIN_WHITE, OUTPUT);

	//Now listening for bluetooth
	BTSerial.attachInterrupt(handleBTChar);
	BTSerial.listen();
}

int counter = 0;

void CMain::Loop()
{
	c1++;
	c2 += 3;

	analogWrite(PIN_RED, c1);
	analogWrite(PIN_GREEN, c2);
	analogWrite(PIN_BLUE, c1);
	analogWrite(PIN_WHITE, c2);

	String str;
	
	if (ReadBTCommand())
	{
		Serial.write((char*)rcvdCmd);
		Serial.println();
		
		str = (char*)rcvdCmd;

		if (str == "next!")
		{
			softwareSerialPort.listen();
			dfPlayer.next();
			delay(20);
			int fileNum = dfPlayer.readCurrentFileNumber();
			
			BTSerial.listen();
			BTSerial.print("File: ");
			BTSerial.print(fileNum);
			BTSerial.println();
			BTSerial.flush();

		}
		if (str == "up!")
		{
			softwareSerialPort.listen();
			//dfPlayer.volumeUp();
			int vol = dfPlayer.readVolume();

			BTSerial.listen();
			BTSerial.print("Vol: ");
			BTSerial.println(vol);
			BTSerial.flush();
		}
		if (str == "down!")
		{
			softwareSerialPort.listen();
			dfPlayer.volumeDown();
			int vol = dfPlayer.readVolume();

			BTSerial.listen();
			BTSerial.print("Vol: ");
			BTSerial.println(vol);
			BTSerial.flush();
		}
	}
	//if (counter++ > 500)
	//{
	//	BTSerial.write("Test!\r\n");
	//	counter = 0;
	//}

	delay(10);
}

void CMain::handleBTChar(uint8_t c)
{
	CMain::Inst.OnBTCharReceived(c);
}

void CMain::OnBTCharReceived(uint8_t c)
{
	if (IsBTCommandComplete())
	{
		// If command is complete, just skip all other symbols until we confirm receive and clean up teh buffer;
		return;
	}
	if (btCmdBufLength >= CMD_MAX_SIZE)
	{
		btCmdBufLength = 0;
	}

	// TODO: add check for start sequence
	// Filling in buffer;
	btCmdBuffer[btCmdBufLength] = c;
	btCmdBufLength++;

}

bool CMain::IsBTCommandComplete()
{
	//TODO: change command completed condition (by length or EOL symbol ?)
	return btCmdBufLength && btCmdBuffer[btCmdBufLength - 1] == '!';
}

bool CMain::ReadBTCommand()
{
	if (!IsBTCommandComplete())
	{
		return false;
	}
	else
	{
		memcpy(rcvdCmd, btCmdBuffer, btCmdBufLength);
		
		//TODO: remove this after changing logic to defined-length packets
		rcvdCmd[btCmdBufLength] = 0;

		btCmdBufLength=0;
		return true;
	}
}
