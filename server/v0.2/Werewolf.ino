#include <Arduino.h>

/**
This is a rework werewolf firmware, using c++ syntex instead
of c syntex, should be much more clear and easy to read

*/

#include "GameLogic.h"
GameLogic game;
void setup() {
	game.init();
}

void loop() {
	game.loop();
}
