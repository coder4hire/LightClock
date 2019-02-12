#pragma once

#define IR_BUF_SIZE 128
#define END_TIMEOUT 64
#define REPEAT_TIMEOUT 128

// Start length in interrupt counts
#define START_LEN 10

enum IR_ACTIONS
{
	IR_NONE=-1,
	IR_CANCEL=0,
	IR_OK,
	IR_UP,
	IR_DOWN,
	IR_LEFT,
	IR_RIGHT,
	IR_MAX
};

// This class is for NEC protocol, SONY protocol might be different
class IRControl
{
public:
	static IRControl Inst;
	~IRControl();

	void Begin();
	void SetupTimerInterrupt();
	void StartReading();
	unsigned long DecodeData();
	IR_ACTIONS IRControl::GetButtonPressed();
	
	//unsigned char buff[IR_BUF_SIZE];
	unsigned long code;
	int bitNum;
	unsigned int counter;
	unsigned int prevPeriodCounter;
	unsigned char prevVal;
	int isFinished;

	void OnInt();

protected:
	IRControl();
	static const unsigned long IRControl::buttonCodes[IR_MAX];
};
