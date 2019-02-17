#include "RGBControl.h"
#include "arduino.h"

CRGBControl CRGBControl::Inst;

const unsigned PROGMEM char CRGBControl::brightness2PWM[256] = {
	0, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5,
	5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9,
	9, 10, 10, 10, 10, 11, 11, 11, 11, 12, 12, 12, 12, 13, 13, 13,
	13, 14, 14, 14, 15, 15, 15, 16, 16, 17, 17, 17, 18, 18, 19, 19,
	19, 20, 20, 21, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26,
	26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 32, 32, 33, 33, 34, 34,
	35, 35, 36, 37, 37, 38, 38, 39, 40, 40, 41, 41, 42, 43, 43, 44,
	45, 45, 46, 47, 47, 48, 49, 49, 50, 51, 52, 52, 53, 54, 55, 55,
	56, 57, 58, 58, 59, 60, 61, 62, 63, 63, 64, 65, 66, 67, 68, 69,
	70, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
	85, 86, 88, 89, 90, 91, 92, 93, 94, 95, 97, 98, 99, 100, 101, 103,
	104, 105, 106, 108, 109, 110, 112, 113, 114, 116, 117, 119, 120, 121, 123, 124,
	126, 127, 129, 130, 132, 133, 135, 136, 138, 140, 141, 143, 144, 146, 148, 150,
	151, 153, 155, 157, 158, 160, 162, 164, 166, 168, 169, 171, 173, 175, 177, 179,
	181, 183, 185, 187, 190, 192, 194, 196, 198, 200, 203, 205, 207, 209, 212, 214,
	216, 219, 221, 224, 226, 229, 231, 234, 236, 239, 242, 244, 247, 250, 252, 255 };

void CRGBControl::Init()
{
	//--- Pins initialization
	pinMode(PIN_RED, OUTPUT);
	pinMode(PIN_GREEN, OUTPUT);
	pinMode(PIN_BLUE, OUTPUT);
	pinMode(PIN_WHITE, OUTPUT);

	// set prescaler to 8 and starts PWM
	//TCCR0B = TCCR0B &(~7) | 4;
	//TCCR1B = TCCR1B &(~7) | 4;
}


CRGBControl::~CRGBControl()
{
}

void CRGBControl::SetRGBW(RGBW rgbw)
{
	analogWrite(PIN_RED, pgm_read_byte(brightness2PWM+rgbw.R));
	analogWrite(PIN_GREEN, pgm_read_byte(brightness2PWM + rgbw.G));
	analogWrite(PIN_BLUE, pgm_read_byte(brightness2PWM + rgbw.B));
	analogWrite(PIN_WHITE, pgm_read_byte(brightness2PWM + rgbw.W));
}
