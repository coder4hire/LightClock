#include <SoftwareSerial.h>
#include "Main.h"

CMain mainInstance;
// the setup function runs once when you press reset or power the board
void setup() 
{
	mainInstance.Setup();
}

// the loop function runs over and over again until power down or reset
void loop() 
{
	mainInstance.Loop();
}
