#include "Main.h"

#include "IRControl.h"
#include "DFRobotDFPlayerMini.h"
#include "RGBControl.h"
#include "Wire.h"

CMain CMain::Inst;

// NOTE: If you use original schematics, this pin should be D7
// But my Nano has this pin broken, so it is rewired to D12
#define BUTTON_PIN 12

CMain::CMain():
	softwareSerialPort(3, 4), // RX, TX
	BTSerial(PIN_A3,2) // RX | TX
{
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
	Wire.begin();

	// Set up RTC clock
	RTC.begin();
	if (!RTC.isrunning()) {
		Serial.println("RTC is NOT running!");
		RTC.adjust(DateTime(__DATE__, __TIME__));
	}
	else
	{
		Serial.println("RTC is running!");
	}

	//Set up DFPlayer
	//dfPlayer.setTimeOut(1000);
	int retries = 10;
	while (!dfPlayer.begin(softwareSerialPort, true, retries == 10) && retries--)
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

	pinMode(BUTTON_PIN, INPUT_PULLUP);

	//Now listening for bluetooth
	BTSerial.attachInterrupt(handleBTChar);
	BTSerial.listen();

	IRControl::Inst.Begin();
	CRGBControl::Inst.Init();

	br = c = 0;

	Serial.println("Regs");
	Serial.println(TCCR0A,16);
	Serial.println(TCCR0B, 16);
	Serial.println(TCCR1A, 16);
	Serial.println(TCCR1B, 16);

	Serial.print("RTC:");
	Serial.print(RTC.now().hour());
	Serial.print(":");
	Serial.print(RTC.now().minute());
	Serial.print(":");
	Serial.print(RTC.now().second());
	Serial.println(":");
}

int counter = 0;

void CMain::Loop()
{
	String str;
	
	if (ReadBTCommand())
	{
		Serial.write((char*)rcvdCmd);
		Serial.println();
		
		str = (char*)rcvdCmd;

		if (str == F("next!"))
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
		if (str == F("up!"))
		{
			softwareSerialPort.listen();
			//dfPlayer.volumeUp();
			int vol = dfPlayer.readVolume();

			BTSerial.listen();
			BTSerial.print("Vol: ");
			BTSerial.println(vol);
			BTSerial.flush();
		}
		if (str == F("down!"))
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

	if (CheckButtonStatus())
	{
		Serial.println(F("Button is pressed"));
		c = 255;

		softwareSerialPort.listen();
		dfPlayer.next();
		BTSerial.listen();

		Serial.println(RTC.isrunning());
	}
	if (counter++ > 500)
	{
	//	BTSerial.write("Test!\r\n");
		counter = 0;
		Serial.print(".");
	}
	unsigned long code = IRControl::Inst.DecodeData();
	if (code)
	{
		Serial.print("IR: ");
		Serial.println(code,16);
	}
	if (code == 0xF807FF00)
	{
		softwareSerialPort.listen();
		dfPlayer.next();
		BTSerial.listen();
	}
	if (code == 0xF906FF00)
	{
		softwareSerialPort.listen();
		dfPlayer.previous();
		BTSerial.listen();
	}

	c++;
	if (!c)
	{
		br = (br + 1) % 4;
	}

	unsigned long val = ((unsigned long)c) << (br * 8);
//	RGBW val;
//	val.R = 1;
//	val.W = 1;
//	Serial.println(val,16);
//	RGBW val;
//	val.R = 1;
//	val.B = 128;
	CRGBControl::Inst.SetRGBW(val);

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

bool CMain::CheckButtonStatus()
{
	pinMode(7, INPUT);
	return !digitalRead(BUTTON_PIN);
}