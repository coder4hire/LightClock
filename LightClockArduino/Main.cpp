#include "Main.h"

#include "DFRobotDFPlayerMini.h"
#include "RGBControl.h"
#include "Wire.h"
#include "BTInterface.h"

CMain CMain::Inst;

// NOTE: If you use original schematics, this pin should be D7
// But my Nano has this pin broken, so it is rewired to D12
#define BUTTON_PIN 12
#define FRONTLIGHT_SENSOR_PIN A6
#define BACKLIGHT_SENSOR_PIN A7

#define LIGHTSTOP_JUMP_THRESHOLD 200

CMain::CMain()
{
	prevButtonState = 0;
	lastFrontReadings = 32768;
}


CMain::~CMain()
{
}

void CMain::Setup()
{
	Serial.begin(115200);
	Wire.begin();

	// Set up RTC clock
	RTC.begin();
	if (!RTC.isrunning()) {
		Serial.println("RTC is NOT running!");
		//RTC.adjust(DateTime(__DATE__, __TIME__));
	}
	else
	{
		Serial.println("RTC is running!");
	}

	// Setting up button pin
	pinMode(BUTTON_PIN, INPUT_PULLUP);
	pinMode(FRONTLIGHT_SENSOR_PIN, INPUT);
	pinMode(BACKLIGHT_SENSOR_PIN, INPUT);

	// Initializing other components
	CBTInterface::Inst.Init();
	IRControl::Inst.Begin();
	CRGBControl::Inst.Init();
	Player.Init();
	Player.SetVolume(5);

	CBTInterface::Inst.Listen();


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

	// Check for front light sensor readings jump
	short frontReadings = GetFrontSensorReadings();
	if(CBoardConfig::Inst.IsStopLightEnabled && frontReadings-lastFrontReadings>LIGHTSTOP_JUMP_THRESHOLD)
	{ 
		CScheduler::Inst.StopEffects();
	}
	lastFrontReadings =frontReadings;

	// Check for remote control button pressed
	IR_ACTIONS actionButton = IRControl::Inst.GetButtonPressed();
	if (actionButton != IR_NONE)
	{
		OnIRButtonPressed(actionButton);
	}

	CBTInterface::Inst.ProcessBTCommands();

	CScheduler::Inst.OnTimeTick();
	CBTInterface::Inst.Listen();

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

short CMain::GetFrontSensorReadings()
{
	return analogRead(FRONTLIGHT_SENSOR_PIN);
}

short CMain::GetBackSensorReadings()
{
	return analogRead(BACKLIGHT_SENSOR_PIN);
}

void CMain::GetSensorsInfo(CSensorsInfo * info)
{
	info->FrontLightSensor = GetFrontSensorReadings();
	info->BackLightSensor = GetBackSensorReadings();
}

bool CMain::CheckButtonStatus()
{
	uint8_t state = digitalRead(BUTTON_PIN);
	if (!state && prevButtonState != state)
	{
		prevButtonState = state;
		return true;
	}
	prevButtonState = state;
	return false;
}

void CMain::OnButtonPressed()
{
	//Serial.println("Button is pressed");
	//CScheduler::Inst.TestRunEffect(CScheduleItem::EF_SUNRISE);
	CScheduler::Inst.StopEffects();
}

void CMain::OnIRButtonPressed(IR_ACTIONS actionButton)
{
	switch (actionButton)
	{
	case IR_LEFT:
		Player.Next();
		break;
	case IR_RIGHT:
		Player.Previous();
		break;
	case IR_UP:
		Player.VolumeUp();
		break;
	case IR_DOWN:
		Player.VolumeDown();
		break;
	case IR_OK:
		Player.PlayRandom();
		break;
	case IR_CANCEL:
		CScheduler::Inst.StopEffects();
		break;
	}
}
