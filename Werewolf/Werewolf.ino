/**
This is a rework werewolf firmware, using c++ syntex instead
of c syntex, should be much more clear and easy to read

*/

#include "GameLogic.h"


#define __VERSION String("V0.2")

GameLogic game;

void setup() {
	Serial.begin(115200);
	game.init();
	game.start();
}

void loop() {
	
}
