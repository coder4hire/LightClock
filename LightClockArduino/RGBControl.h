#pragma once

union RGBW
{
	unsigned long Data;
	struct 
	{
		unsigned char R;
		unsigned char G;
		unsigned char B;
		unsigned char W;
	};

	RGBW(unsigned long data=0)
	{
		Data = data;
	}
};

class CRGBControl
{
public:
	enum PINS
	{
		PIN_RED = 5,
		PIN_GREEN = 6,
		PIN_BLUE = 9,
		PIN_WHITE = 10
	};

	static CRGBControl Inst;

	void CRGBControl::Init();
	~CRGBControl();
	void SetRGBW(RGBW rgbw);
protected:
	static const unsigned char brightness2PWM[256];
	CRGBControl() {}
};

