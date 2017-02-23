#ifndef GameLogic_h
#define GameLogic_h

#include "Communication.h"
#include "Arduino.h"
#include "Timer.h"
#include "hardware.h"
/**
*
* Game Configuation
*/
#define PLAYER_NUMBER 12 // should be less than 16 player
#define LYCAN_NUMBER 4
#define HAS_WITCH true
#define HAS_SEER true
#define HAS_HUNTER true
#define HAS_MORON true
#define HAS_GUARD false
#define __VERSION 0.2
// time definition
#define XS_TIME 1000
#define S_TIME 2000
#define M_TIME 4000
#define L_TIME 6000
#define XL_TIME 10000
#define SELECT_TIME 60000
#define SPEECH_SPEED 100
#define SPEECH_TIME 90000
#define CONFIRM_TIME 60000
/**
	Which output/intput we are using
*/




/**
* Structure definition
*/
typedef enum{R_CITIZEN,R_LYCAN,R_WITCH,R_HUNTER,R_SEER,R_MORON,R_GUARD,R_ALL} role_t;

typedef uint8_t Pid;

typedef struct{

	uint16_t playerAlive;
	role_t playerRole[PLAYER_NUMBER];
	Pid hunterId;
	Pid guardId;
	Pid witchId;
	Pid seerId;
	Pid moronId;
	Pid sheriffId;
	bool isFirstLoop;
	bool usedPosion;
	bool usedCure;
	bool badgeLost;
} GameStatus;

typedef struct
{
	Pid lycanKillId;
	Pid witchPosionId;
	bool witchSaved;
	Pid suspectId;

} TurnStatus;


/**
	Main Game Logic class
	All game logic goes here...
*/

class GameLogic
{
	public:
		GameLogic();
		~GameLogic();

		void init();
		void loop();

		void say(String);
	private:
		Driver *conn;
		GameStatus *status;
		TurnStatus *tstatus;

		// game helper
		void checkResult();
		Pid previousAlivePlayer(Pid select);
		Pid nextAlivePlayer(Pid select);
		uint16_t clientIdToBinary(Pid id);

		bool isPlayerAlive(Pid id);
		void markPlayerDie(Pid id);
		void markPlayerAlive(Pid id);

		// logic proc
		void lycanTurn();
		void witchTurn();
		void seerTurn();
		void hunterTurn();
		void moronTurn();
		void sheirffCampagin();
		void changeSheirff();
		void hunterSkill();
		void moronSkill();

		void reportSuvivor();
		void reportVictim(uint16_t deadList);
		void voteForSuspect();
		void startSpeech();
		// main loop
		void onNight();
		void onDay();

		// input/output
		Pid confirmOneIdentity(bool usingGreenLight);
		Pid roleChangeOnce(role_t from,role_t to);
		void roleChangeMore(role_t from,role_t to,uint8_t count);
		bool confirmWithId(Pid allow, unsigned long timeout,Pid lightsOn);
		bool confirmWithRole(role_t allow, unsigned long timeout,Pid lightsOn);
		Pid selectOneWithAllowRole(unsigned long timeout,role_t allow,bool usingGreenLight);
		Pid selectOneWithAllowId(unsigned long timeout,Pid id,bool usingGreenLight);

		// light control
		void powerOnAllLight();
		void powerOffAllLight();
		void showIdentity();

		// self check
		void checkClient();
};

#endif
