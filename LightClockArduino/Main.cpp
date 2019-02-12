#include "Main.h"

#include "DFRobotDFPlayerMini.h"
#include "RGBControl.h"
#include "Wire.h"
#include "BTInterface.h"

CMain CMain::Inst;

// NOTE: If you use original schematics, this pin should be D7
// But my Nano has this pin broken, so it is rewired to D12
#define BUTTON_PIN 12

CMain::CMain():
	softwareSerialPort(3, 4) // RX, TX
{
}


CMain::~CMain()
{
}

void CMain::Setup()
{
	Serial.begin(115200);
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

	// Setting up button pin
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	// Initializing other components
	CBTInterface::Inst.Init();
	IRControl::Inst.Begin();
	CRGBControl::Inst.Init();


	/*dfPlayer.volume(2);
	dfPlayer.play();
	Serial.print("Vol: ");
	Serial.println(dfPlayer.readVolume());
	softwareSerialPort.listen();
	Serial.print("Vol: ");
	Serial.println(dfPlayer.readVolume());
	Serial.print("Track: ");
	Serial.println(dfPlayer.readCurrentFileNumber());

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
	Serial.println(":"); */
}

void CMain::Loop()
{
	String str;
	
	/*if (ReadBTCommand())
	{
		Serial.write((char*)rcvdCmd);
		Serial.println();
		
		str = (char*)rcvdCmd;

		/ (str == F("next!"))
		{
			softwareSerialPort.listen();
			dfPlayer.next();
			delay(20);
			int fileNum = dfPlayer.readCurrentFileNumber();
			
			CBTInterface::Inst.Listen();
			//BTSerial.print("File: ");
			BTSerial.print(fileNum);
			BTSerial.println();
			BTSerial.flush();

		}
		if (str == F("up!"))
		{
			softwareSerialPort.listen();
			dfPlayer.volumeUp();
			int vol = dfPlayer.readVolume();

			CBTInterface::Inst.Listen();
			BTSerial.print("Vol: ");
			BTSerial.println(vol);
			BTSerial.flush();
		}
		if (str == F("down!"))
		{
			softwareSerialPort.listen();
			dfPlayer.volumeDown();
			int vol = dfPlayer.readVolume();

			CBTInterface::Inst.Listen();
			BTSerial.print("Vol: ");
			BTSerial.println(vol);
			BTSerial.flush();
		}
	}*/

	if (CheckButtonStatus())
	{
		OnButtonPressed();
	}

	// Check for remote control button pressed
	IR_ACTIONS actionButton = IRControl::Inst.GetButtonPressed();
	if (actionButton != IR_NONE)
	{
		OnIRButtonPressed(actionButton);
	}

	CBTInterface::Inst.ProcessBTCommands();

/*	c++;
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
	CRGBControl::Inst.SetRGBW(val);*/

	delay(10);
}

bool CMain::CheckButtonStatus()
{
	pinMode(7, INPUT);
	return !digitalRead(BUTTON_PIN);
}

void CMain::OnButtonPressed()
{
	softwareSerialPort.listen();
	dfPlayer.next();
	CBTInterface::Inst.Listen();
}

void CMain::OnIRButtonPressed(IR_ACTIONS actionButton)
{
	switch (actionButton)
	{
	case IR_LEFT:
		softwareSerialPort.listen();
		dfPlayer.next();
		CBTInterface::Inst.Listen();
		break;
	case IR_RIGHT:
		softwareSerialPort.listen();
		dfPlayer.previous();
		CBTInterface::Inst.Listen();
		break;
	}
}
