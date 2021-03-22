#include "IRControl.h"
#include "Arduino.h"

IRControl IRControl::Inst;

const PROGMEM unsigned long IRControl::buttonCodes[IR_MAX] =
{
	0xB04FFF00, // IR_CANCEL (Menu)
	0xF30CFF00, // IR_OK
	0xF10EFF00, // IR_UP
	0xF609FF00, // IR_DOWN
	0xF20DFF00, // IR_LEFT
	0xF40BFF00, // IR_RIGHT
	0xE817FF00, // IR_REPEAT
	0xFF00FF00, // IR_PLAY
	0xFE01FF00, // IR_STOP
	0xB34CFF00, // IR_POWER
	0x11111111, // IR_LIGHT
};

IRControl::IRControl()
{
	pinMode(PIN_A0, INPUT_PULLUP);
	bitNum = -1;
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
	TCCR2A = 0;
	TCCR2B = 0;

	TCNT2 = (255-12);            // preload timer 65536 - for 0.2ms interval
	TCCR2B |= (1 << CS22) | (1 << CS21);    // 1/256 prescaler 
	TIMSK2 |= (1 << TOIE2);   // enable timer overflow interrupt
	interrupts();             // enable all interrupts
}

void IRControl::StartReading()
{
	prevVal = 1;
	prevPeriodCounter = 0;
	counter = 0;
	bitNum = -1;
	code = 0;
	isFinished = false;
}

unsigned long IRControl::DecodeData()
{
	if(isFinished)
	{
		unsigned long retVal = code;
		StartReading();
		return retVal;
	}
	return 0;
}

IR_ACTIONS IRControl::GetButtonPressed()
{
	unsigned long code = IRControl::DecodeData();
	if (code)
	{
		for (int i = 0; i < IR_MAX; i++)
		{
			if (pgm_read_dword(buttonCodes+i) == code)
			{
				Serial.print("IR button pressed ");
				Serial.println(i);
				return (IR_ACTIONS)i;
			}
		}
		Serial.println(code, 16);
	}
	return IR_NONE;
}


ISR(TIMER2_OVF_vect)        // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
	IRControl::Inst.OnInt();
}

void IRControl::OnInt()
{
	TCNT2 = (255 - 12);            // preload timer
	if (isFinished)
	{
		return;
	}

	unsigned char val = PINC & 1; // Bit0 corresponds to PIN_A0
	if (counter > END_TIMEOUT)
	{
		if (bitNum >= 0)
		{
			isFinished = true;
			return;
		}
	}
	else
	{
		counter++;
	}


	if (val != prevVal)
	{
		if (bitNum == -1)
		{
			// Looking for start bit
			if (prevVal == 1 && prevPeriodCounter >= 20 )
			{
				bitNum = 0;
			}
		}
		else
		{
			if (prevVal == 1)
			{
				if (prevPeriodCounter >= 3 && prevPeriodCounter < 9 &&	// low half of bit sequence
					counter >= 2 && counter < 15)						// high half of bit sequence
				{
					if (counter >= 7)
					{
						// This is "1" bit
						code = code | (1L << bitNum);
					}
					bitNum++;
					if (bitNum >= sizeof(code) * 8)
					{
						isFinished = true;
						return;
					}
				}
				else
				{
					StartReading(); // Error, restart reading
				}
			}
		}

		prevVal = val;
		prevPeriodCounter = counter;
		counter = 0;
	}
}