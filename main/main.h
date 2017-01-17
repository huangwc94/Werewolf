/**
* Software Info
*/

#define __NAME String("Werewolf")
#define __VERSION String("V0.1")
#define __SERVER_ID 1234
#define STOP_CHAR '$'

/**
* If in test mode, comment out for normal game mode
*/
//#define TEST_MODE

/**
* How many player in this game
*/
#define PLAYER_NUMBER 12
#define LYCAN_NUMBER 4

/**
* Role definition
*/
typedef enum{R_CITIZEN,R_LYCAN,R_WITCH,R_HUNTER,R_SEER,R_MORON} role_t;

/**
* Player ID
*/
#define ID_1 1
#define ID_2 1 <<1
#define ID_3 1 <<2
#define ID_4 1 <<3
#define ID_5 1 <<4
#define ID_6 1 <<5
#define ID_7 1 <<6
#define ID_8 1 <<7
#define ID_9 1 <<8
#define ID_10 1 <<9
#define ID_11 1 <<10
#define ID_12 1 <<11
#define ID_ALL 65535
