#include "Main.h"

// the setup function runs once when you press reset or power the board
void setup() 
{
	CMain::Inst.Setup();
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	CMain::Inst.Loop();
}
