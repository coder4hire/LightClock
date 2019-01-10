#include "Main.h"

#include "DFRobotDFPlayerMini.h"

CMain::CMain():
	softwareSerialPort(3, 4), // RX, TX
	BTSerial(PIN_A3,2) // RX | TX
{
	c1 = 0;
	c2 = 0;
}


CMain::~CMain()
{
}

void CMain::Setup()
{
	Serial.begin(115200);
	BTSerial.begin(9600);
	softwareSerialPort.begin(9600);
	
	int retries = 10;
	while (!dfPlayer.begin(softwareSerialPort,true,retries==10) && retries--)
	{  
		TRACE(F("Unable to connect to DFPlayer"));
	}


	dfPlayer.volume(5);
	dfPlayer.play();
	Serial.print("Vol: ");
	Serial.println(dfPlayer.readVolume());

	//--- Pins initialization
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_GREEN, OUTPUT);
	pinMode(PIN_BLUE, OUTPUT);
	pinMode(PIN_WHITE, OUTPUT);
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
	BTSerial.listen();
	if (BTSerial.available())
	{
		while (BTSerial.available())
		{
			int ch = BTSerial.read();
			str += (char)ch;
		}
		Serial.write(str.c_str());
		Serial.println();

		if (str == "next")
		{
			softwareSerialPort.listen();
			delay(10);
			dfPlayer.next();
			BTSerial.print("File: ");
			BTSerial.println(dfPlayer.readCurrentFileNumber());
		}
		if (str == "up")
		{
			softwareSerialPort.listen();
			delay(10);
			dfPlayer.volumeUp();
			BTSerial.print("Vol: ");
			BTSerial.println(dfPlayer.readVolume());
		}
		if (str == "down")
		{
			softwareSerialPort.listen();
			delay(10);
			dfPlayer.volumeDown();
			BTSerial.print("Vol: ");
			BTSerial.println(dfPlayer.readVolume());
		}
	}
	BTSerial.listen();
	//if (counter++ > 1000)
	//{
	//	BTSerial.write("Test!\r\n");
	//	counter = 0;
	//}

	delay(10);
}
