#pragma once

#define IR_BUF_SIZE 128
#define END_TIMEOUT 256
#define REPEAT_TIMEOUT 128

enum IR_ACTIONS
{
	IR_STOP,

};

class IRControl
{
public:
	static IRControl Inst;
	~IRControl();

	void Begin();
	void SetupTimerInterrupt();
	void StartReading();
	int DecodeData();
	
	unsigned char buff[IR_BUF_SIZE];
	int bufSizeInBits;
	int highCounter;
	int isFinished;

protected:
	IRControl();
};

