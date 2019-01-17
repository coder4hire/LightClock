#include "IRControl.h"
#include "Arduino.h"

IRControl IRControl::Inst;

IRControl::IRControl()
{
	pinMode(PIN_A0, INPUT);
	bufSizeInBits = 0;
	highCounter = 0;
	isFinished = false;
}


IRControl::~IRControl()
{
}

void IRControl::Begin()
{
	StartReading();
	SetupTimerInterrupt();
}

void IRControl::SetupTimerInterrupt()
{

	// initialize timer1 
	noInterrupts();           // disable all interrupts
	TCCR1A = 0;
	TCCR1B = 0;

	TCNT1 = (65536-3200);            // preload timer 65536 - for 0.2ms interval
	TCCR1B |= (1 << CS10);    // 1 prescaler 
	TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
	interrupts();             // enable all interrupts
}

void IRControl::StartReading()
{
	highCounter = 0;
	bufSizeInBits = 0;
	isFinished = false;
	Serial.println(F("IR Start"));
}

int IRControl::DecodeData()
{
	if (isFinished)
	{
		for (int i = 0; i < bufSizeInBits / 8; i++)
		{
			Serial.print(buff[i]);
			Serial.print(" ");
		}
		Serial.println("");
		StartReading();
	}
}

ISR(TIMER1_OVF_vect)        // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
	//cli();
	TCNT1 = (65536 - 3200);            // preload timer
	if (IRControl::Inst.isFinished)
	{
		return;
	}

	if (IRControl::Inst.bufSizeInBits == IR_BUF_SIZE * 8)
	{
		IRControl::Inst.isFinished = true;
		return;
	}

	unsigned char val = PINC & 1; // Bit0 corresponds to PIN_A0
	//unsigned char val = digitalRead(PIN_A0);
	if (val && IRControl::Inst.bufSizeInBits == 0)
	{
		return;
	}

	// Storing data into buffer
	int bitNum = IRControl::Inst.bufSizeInBits & 0x7;
	int byteNum = IRControl::Inst.bufSizeInBits >> 3;
	if (!bitNum)
	{
		IRControl::Inst.buff[byteNum] = 0;
	}
	IRControl::Inst.buff[byteNum] |= (val << bitNum);
	IRControl::Inst.bufSizeInBits++;

	// Checking for timeouts
	if(val)
	{
		IRControl::Inst.highCounter++;
		if (IRControl::Inst.highCounter > END_TIMEOUT)
		{
			IRControl::Inst.isFinished = true;
		}
	}
	else
	{
		IRControl::Inst.highCounter = 0;
	}
	//sei();
}
