#ifndef GameLogic_h
#define GameLogic_h

#include "Communication.h"
#include "Arduino.h"

/**
* Role definition
*/
typedef enum{R_CITIZEN,R_LYCAN,R_WITCH,R_HUNTER,R_SEER,R_MORON,R_GUARD} role_t;


/**
* How many player in this game
*/
#define PLAYER_NUMBER 12
#define LYCAN_NUMBER 4




class GameLogic
{
	public:
		GameLogic();
		void init();
		void init();

	private:
		role_t playerRole[PLAYER_NUMBER];
};

#endif