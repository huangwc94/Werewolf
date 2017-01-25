/**
 * Author: Weicheng Huang
 */
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "main.h"

/**
 * Redio Setting and init
 */
RF24 radio(9,10);
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

uint16_t g_playerAlive;
role_t  g_playerRole[12];

uint16_t g_playerRedLight;
uint16_t g_playerGreenLight;

uint8_t g_stage;
unsigned long g_checkTime;
bool g_isFirstLoop;

bool g_witchPosionUsed;
bool g_witchCureUsed;
uint8_t g_witchId;
uint8_t g_seerId;
uint8_t g_moronId;
uint8_t g_hunterId;
uint8_t g_sheriffId;

String inputString = "";
/********************************************************************************
 * Game Logic
 ********************************************************************************/
void showIdentity(){
	powerOffAllLight();
	for(int i = 0;i<PLAYER_NUMBER;i++){
		if(g_playerRole[i] == R_LYCAN)
			g_playerRedLight |= clientIdToBinary(i+1);
		else
			g_playerGreenLight |= clientIdToBinary(i+1);
	}
	renderLight();
}

void checkResult(){
  uint8_t werewolfRemain = 0;
  uint8_t citizenRemain = 0;
  uint8_t godRemain = 0;

  for(int i = 0;i<PLAYER_NUMBER;i++){

  	if((g_playerAlive & clientIdToBinary(i+1)) == 0)
  		continue;

  	if(g_playerRole[i] == R_LYCAN)
  		werewolfRemain++;
  	else if(g_playerRole[i] == R_CITIZEN)
  		citizenRemain++;
  	else
  		godRemain++;
  }

  if(werewolfRemain==0 && (citizenRemain==0 || godRemain==0)){
  	Serial.println("Game Over, the result is");
  	delay(5000);
  	Serial.println("Draw, everyone win");
  	showIdentity();
  	while(1);
  }

  if(werewolfRemain==0){
  	Serial.println("Game Over, the result is");
  	delay(5000);
  	Serial.println("Good Man team win!");
  	showIdentity();
  	while(1);
  }

  if(citizenRemain==0 || godRemain==0){
  	Serial.println("Game Over, the result is");
  	delay(5000);
  	Serial.println("Werewolf team win!");
  	showIdentity();
  	while(1);
  }
  //Serial.println(String("Game is not over! ") + werewolfRemain + " werewolves remain. " + citizenRemain + " citizens remain. " + godRemain + " god remain.");
  delay(5000);
}

bool confirm(unsigned long timeout,role_t allow){
  	unsigned long startTime = millis();
  	unsigned long current = millis();

  	uint8_t id,btn;
  
	while((current - startTime) < timeout || timeout == 0){
	    current = millis();
	    if(getInput(&id,&btn) && g_playerRole[id - 1] == allow){
	    	if(btn == 3) return true;
	    	else if(btn == 4) return false;
	    }
	}
	return false;
}
void sheriffSelectNew(){
	powerOffAllLight();
	Serial.println("Our sheriff was dead, please select a new sheriff");
	g_playerAlive = g_playerAlive | clientIdToBinary(g_sheriffId);
	delay(5000);
	uint8_t newg_sheriffId = selectOneWithAllowId(0,g_sheriffId);
	g_playerAlive = g_playerAlive & ~clientIdToBinary(g_sheriffId);
	g_sheriffId = newg_sheriffId;
	Serial.println(String("New sheriff will be:") + g_sheriffId);
	delay(3000);
	g_playerGreenLight = clientIdToBinary(g_sheriffId);
	renderLight();
	delay(3000);
	powerOffAllLight();
}

uint8_t selectOne(unsigned long timeout, role_t allow){
	uint8_t select = 0;
	powerOffAllLight();
	unsigned long startTime = millis();
	unsigned long current = millis();
	uint8_t id,btn;

	while((current - startTime) < timeout || timeout == 0){
		current = millis();
		if(getInput(&id,&btn)){
			if(g_playerRole[id - 1] == allow && (g_playerAlive & clientIdToBinary(id)) != 0){
				if(btn == 3){
					powerOffAllLight();
					return select;
				}else if(btn==1){
					select = previousAlivePlayer(select);
					g_playerRedLight = clientIdToBinary(select);
					renderLight();
				}else if(btn==2){
					select = nextAlivePlayer(select);
					g_playerRedLight = clientIdToBinary(select);
					renderLight();
				}else if(btn==4){
					select = 0;
					powerOffAllLight();
					return select;
				}
			}
		}
	}
	powerOffAllLight();
	return select;
}

uint8_t selectOneWithAllowId(unsigned long timeout, uint8_t allowId){

	uint8_t select = 0;
	powerOffAllLight();
	unsigned long startTime = millis();
	unsigned long current = millis();
	uint8_t id,btn;

	while((current - startTime) < timeout || timeout == 0){
		current = millis();
		if(getInput(&id,&btn)){
			if(id == allowId && (g_playerAlive & clientIdToBinary(id)) != 0){
				if(btn == 3){
					powerOffAllLight();
					return select;
				}else if(btn==1){
					select = previousAlivePlayer(select);
					g_playerRedLight = clientIdToBinary(select);
					renderLight();
				}else if(btn==2){
					select = nextAlivePlayer(select);
					g_playerRedLight = clientIdToBinary(select);
					renderLight();
				}else if(btn==4){
					select = 0;
					powerOffAllLight();
					return select;
				}
			}
		}
	}
	powerOffAllLight();
	return select;
}

uint8_t previousAlivePlayer(uint8_t select){
	if(g_playerAlive == 0){
		return 0;
	}
	select--;
	if(select <= 0){
		select = PLAYER_NUMBER;  
	}
	while((g_playerAlive & clientIdToBinary(select)) == 0){
		select--;
		if(select <= 0){
			select = PLAYER_NUMBER;  
		}
	}

	return select;
}
uint8_t nextAlivePlayer(uint8_t select){
	if(g_playerAlive == 0){
		return 0;
	}
	select++;
	if(select >PLAYER_NUMBER){
		select = 1;
	}
	while((g_playerAlive & clientIdToBinary(select)) == 0){
		select++;
		if(select > PLAYER_NUMBER){
			select = 1;
		}
	}
	return select;
}


/********************************************************************************
 * End Game Logic
 ********************************************************************************/

 
/********************************************************************************
* Driver
********************************************************************************/
/**
 * Play Music
 */
void play(int music_id){
	Serial.println(String("PLAY:") + music_id);
}

/**
 * Send light information to client
 */
void renderLight(){
	Serial.println(String("RLIGHT:")+g_playerRedLight);
	Serial.println(String("GLIGHT:")+g_playerGreenLight);
}

void powerOffAllLight(){
	g_playerRedLight = 0;
	g_playerGreenLight = 0;
	renderLight();
}

/********************************************************************************
* End Driver
********************************************************************************/

/********************************************************************************
* Input/Output
********************************************************************************/
/**
 * Client Input Handler
 * 
 * btn = 1 left
 * btn = 2 right
 * btn = 3 yes
 * btn = 4 no
 * 
 * client id binary
 */
void clearInput(){
	inputString = String("");
	Serial.flush();
}

bool getInput(uint8_t *clientId, uint8_t *btn){
	if(Serial.available()) {
		// get the new byte:
		char inChar = (char)Serial.read();
		if (inChar == STOP_CHAR) {
			*clientId = (uint8_t) inputString[0] - 'a' + 1;
			*btn =      (uint8_t) inputString[2] - 'a' + 1;
			clearInput();
			return true;
		}else{
			inputString += inChar;
			return false;
		}
	}
	return false;
}

uint16_t clientIdToBinary(uint8_t id){
	if(id>0)
		return 1 << (id - 1);
	else
		return 0;
}

/********************************************************************************
* End Input/Output
********************************************************************************/


/********************************************************************************
* Program Entry
********************************************************************************/
void setup() {
	Serial.begin(115200);
	radio.begin();
	delay(100);
	Serial.println("Welcome to Werewolf " + __VERSION);
	/**
	* Init variables
	*/
	g_playerAlive = 0xFFF;
	for(int i = 0;i<PLAYER_NUMBER;i++){
		g_playerRole[i] = R_CITIZEN;
	}
	g_playerRedLight = 0xFFF;
	g_playerGreenLight = 0xFFF;
	renderLight();
	g_checkTime = millis();
	g_isFirstLoop = true;

	g_witchPosionUsed = false;
	g_witchCureUsed = false;

	/**
	* Init successfuly
	* 
	**/

	delay(4000);
	Serial.println("Game Started");
	delay(2000);
	g_playerRedLight = 0;
	g_playerGreenLight = 0;
	renderLight();

	#ifdef TEST_MODE
		Serial.println("Running in test mode...");
		test();
		while(1);
	#endif
}



void loop() {
	gameLoop();
}
void gameLoop(){
	uint8_t lycan_kill_id = 0;
	uint8_t witch_kill_id = 0;
	bool hunter_enable_skill = false;
	uint8_t id,btn;
	uint8_t suspectId = 0;

	Serial.println("Night is coming, all close eyes");
	powerOffAllLight();
	delay(3000);

	/**
	* Lycan turn
	*/
	Serial.println("Werewolf open eyes");
	delay(3000);
	if(g_isFirstLoop){
		Serial.println("Werewolf confirm identity");
		uint8_t confirmCount = 0;
		clearInput();
		while(confirmCount < LYCAN_NUMBER){
			if( getInput(&id,&btn) && (g_playerRole[id - 1] != R_LYCAN)){
				confirmCount ++;
				g_playerRole[id - 1] = R_LYCAN;
				g_playerRedLight += clientIdToBinary(id);
				renderLight();
				//Serial.println(String("Werewolf confirmed:") + id);
			}
		}
	}
	delay(1000);
	powerOffAllLight();
	Serial.println("Werewolf pick someone to kill");
	lycan_kill_id = selectOne(30000,R_LYCAN);
	//Serial.println(String("Werewolf select to kill:") + lycan_kill_id);
	delay(3000);
	Serial.println("Werewolf close eyes");
	powerOffAllLight();
	delay(3000);
	/**
	* Witch
	*/
	Serial.println("Witch open eyes...");
	delay(3000);
	if(g_isFirstLoop){
		Serial.println("Witch confirm identity...");
		delay(2000);
		clearInput();
		while(1){
			if(getInput(&id,&btn) && (g_playerRole[id - 1] == R_CITIZEN)){
				g_playerRole[id - 1] = R_WITCH;
				g_witchId = id;
				//Serial.println("Witch confirmed:" + id);
				g_playerGreenLight = clientIdToBinary(id);
				renderLight();
				delay(2000);
				break;
			}
		}
	}
	powerOffAllLight();

	bool witchDie = (g_playerAlive & clientIdToBinary(g_witchId)) == 0;
	Serial.println(String("Tonight's victim is:"));
	delay(2000);
	if(witchDie){
		Serial.println("Do you want to save this guy?");
		delay(9000);
		Serial.println("Do you want to posion someone?");
		delay(10000);
	}else{
  
		g_playerGreenLight = 0;
		g_playerRedLight = g_witchCureUsed ? 0 : clientIdToBinary(lycan_kill_id);
		renderLight();
		delay(3000);
		Serial.println("Do you want to save this guy?");
		// Save victim
		bool saveSuccess = false;
		if(!g_witchCureUsed){
			if(lycan_kill_id == g_witchId){
				if(g_isFirstLoop){
					g_playerGreenLight = clientIdToBinary(lycan_kill_id);
					g_playerRedLight = clientIdToBinary(lycan_kill_id);
					renderLight();
					clearInput();
					if(confirm(30000,R_WITCH)){
						g_witchCureUsed = true;
						lycan_kill_id = 0;
						g_playerRedLight = 0;
						renderLight();
						saveSuccess = true;
					}else{
						g_playerGreenLight = 0;
						renderLight();
					}
				}else{
					delay(3000);
				}
			}else if(lycan_kill_id > 0){
				g_playerGreenLight = clientIdToBinary(lycan_kill_id);
				g_playerRedLight = clientIdToBinary(lycan_kill_id);
				renderLight();
				clearInput();
				if(confirm(30000,R_WITCH)){
					g_witchCureUsed = true;
					lycan_kill_id = 0;
					g_playerRedLight = 0;
					renderLight();
					saveSuccess = true;
				}else{
					g_playerGreenLight = 0;
					renderLight();
				}
			}
			delay(1000);
		}else{
			delay(4000);
		}

		powerOffAllLight();
		delay(1000);
		Serial.println("Do you want to posion someone?");
		if(g_witchPosionUsed || saveSuccess){
			delay(10000);
		}else{
			clearInput();
			witch_kill_id = selectOne(30000,R_WITCH);
			g_witchPosionUsed = true;
		}
	}
	Serial.println("Witch close eyes");
	powerOffAllLight();

	/**
	* R_SEER turn
	*/
	delay(3000);
	Serial.println("Seer open eyes");
	delay(2000);
	if(g_isFirstLoop){
		Serial.println("Seer confirm identity...");
		delay(2000);
		clearInput();
		while(1){
			if(getInput(&id,&btn) && (g_playerRole[id - 1] == R_CITIZEN)){
				g_playerRole[id - 1] = R_SEER;
				g_seerId = id;
				//Serial.println("Seer confirmed:" + id);
				g_playerGreenLight = clientIdToBinary(id);
				renderLight();
				delay(2000);
				break;
			}
		}
	}
 	bool seerDie = (g_playerAlive & clientIdToBinary(g_seerId)) == 0;
	if(seerDie){
		Serial.println("Seer pick someone to testify");
		delay(5000);
		Serial.println("The result of this guys is");
		delay(3000);
	}else{
		Serial.println("Seer pick someone to testify");
		clearInput();
		uint8_t select = selectOne(30000,R_SEER);
		bool isBadGuy = g_playerRole[select - 1] == R_LYCAN;
		Serial.println("The result of this guys is");
		delay(1000);
		if(isBadGuy){
			g_playerRedLight = clientIdToBinary(select);
		}else{
			g_playerGreenLight = clientIdToBinary(select);
		}
		renderLight();
		delay(3000);
	}
	powerOffAllLight();
	Serial.println("Seer close eyes");
	delay(3000);

	Serial.println("Hunter open eyes");
	delay(2000);
	/**
	R_Hunter turn
	*/
	if(g_isFirstLoop){
		Serial.println("Hunter confirm identity");
		delay(2000);
		clearInput();
		while(1){
			if(getInput(&id,&btn) && (g_playerRole[id - 1] == R_CITIZEN)){
				g_playerRole[id - 1] = R_HUNTER;
				g_hunterId = id;
				//Serial.println("Hunter confirmed:" + id);
				g_playerGreenLight = clientIdToBinary(id);
				renderLight();
				delay(2000);
				break;
			}
		}
	}
	powerOffAllLight();
	bool hunterDie = (g_playerAlive & clientIdToBinary(g_hunterId)) == 0;
	if(hunterDie){
		Serial.println("Tonight you are");
		delay(1000);
		Serial.println("to shot someone");
		delay(3000);
		Serial.println("Do you want to use your ability?");
		delay(10000);
	}else{
		if(lycan_kill_id == g_hunterId && witch_kill_id != g_hunterId){
			Serial.println("Tonight you are");
			g_playerGreenLight = clientIdToBinary(g_hunterId);
			renderLight();
			delay(1000);
			Serial.println("to shot someone");
			delay(3000);
			powerOffAllLight();
			Serial.println("Do you want to use your ability?");
			delay(2000);
			g_playerGreenLight = clientIdToBinary(g_hunterId);
			g_playerRedLight = clientIdToBinary(g_hunterId);
			renderLight();
			clearInput();
			hunter_enable_skill = confirm(30000,R_HUNTER);
			if(hunter_enable_skill){
				g_playerRedLight = 0;
			}else{
				g_playerGreenLight = 0;
			}
			renderLight();
			delay(2000);
		}else{
			Serial.println("Tonight you are");
			g_playerRedLight = clientIdToBinary(g_hunterId);
			renderLight();
			delay(500);
			Serial.println("to shot someone");
			delay(3000);
			Serial.println("Do you want to use your ability?");
			powerOffAllLight();
			delay(10000);
		}
	}
	powerOffAllLight();

	Serial.println("Hunter close eyes");
	delay(3000);

	/**
	R_Moron turn
	*/
	if(g_isFirstLoop){
		Serial.println("Moron open eyes and confirm identity");
		delay(2000);
		clearInput();
		while(1){
			if(getInput(&id,&btn) && (g_playerRole[id - 1] == R_CITIZEN)){
				g_playerRole[id - 1] = R_MORON;
				g_moronId = id;
				//Serial.println("Moron confirmed:" + id);
				g_playerGreenLight = clientIdToBinary(id);
				renderLight();
				delay(2000);
				Serial.println("Moron close eyes");
				delay(3000);
				break;
			}
		}
	}
	powerOffAllLight();
	
	Serial.println("The night is over, all open eyes");
	delay(4000);
	/**
	* Sheriff Campain
	*
	*
	*/
	if(g_isFirstLoop){
		Serial.println("Now we start the sheriff campaign");
		delay(3000);
		Serial.println("After complete, sheriff please click confirm key...");
		delay(4000);
		clearInput();
		while(1){
			if(getInput(&id,&btn) &&  (g_playerAlive & clientIdToBinary(id)) != 0){
				g_sheriffId = id;
				g_playerGreenLight = clientIdToBinary(g_sheriffId);
				renderLight();
				break;
			}

		}
		Serial.println(String("Today's sheriff is:")+g_sheriffId);
		delay(3000);
		powerOffAllLight();
	}


	g_playerRedLight = clientIdToBinary(lycan_kill_id) | clientIdToBinary(witch_kill_id);
	g_playerGreenLight = 0;
	if(g_playerRedLight > 0){
		Serial.println("Yesterday's victim are");
		delay(1000);
		renderLight();
	}else{
		Serial.println("Yesterday nobody die");
	}

	delay(3000);
	g_playerAlive = g_playerAlive & ~clientIdToBinary(lycan_kill_id);
	g_playerAlive = g_playerAlive & ~clientIdToBinary(witch_kill_id);

	if(hunter_enable_skill){
		Serial.println("Hunter please pick one to kill");
		g_playerAlive = g_playerAlive | clientIdToBinary(g_hunterId);
		delay(2000);
		clearInput();
		uint8_t hunter_kill_id = selectOne(30000,R_HUNTER);

		g_playerAlive = g_playerAlive & ~clientIdToBinary(hunter_kill_id);
		g_playerAlive = g_playerAlive & ~clientIdToBinary(g_hunterId);
	}

	delay(2000);
	powerOffAllLight();
	g_playerGreenLight = g_playerAlive;
	Serial.println("Today's survivors are");
	delay(3000);
	renderLight();

	checkResult();

	bool sheriffDie = (g_playerAlive & clientIdToBinary(g_sheriffId)) == 0;
	if(sheriffDie){
		clearInput();
		sheriffSelectNew();
	}
	powerOffAllLight();
	Serial.println("Now start free talking");
	delay(3000);
	Serial.println("After complete, the suspect will click confirm key");
	g_playerGreenLight = g_playerAlive;
	renderLight();
	delay(4000);
	clearInput();
	while(1){
		if(getInput(&id,&btn) &&  (g_playerAlive & clientIdToBinary(id)) != 0){
			suspectId = id;
			g_playerRedLight = clientIdToBinary(id);
			g_playerGreenLight = 0;
			renderLight();
			break;
		}
	}
	Serial.println(String("The suspect is ") + suspectId);
	delay(3000);
	powerOffAllLight();

	if(suspectId == g_hunterId){
		Serial.println("Do you want to use your ability?");
		g_playerRedLight = clientIdToBinary(suspectId);
		g_playerGreenLight = clientIdToBinary(suspectId);
		renderLight();
		delay(3000);
		clearInput();
		if(confirm(30000,R_HUNTER)){
			g_playerRedLight = 0;
			renderLight();
			Serial.println("Hunter please pick one to kill");

			delay(2000);
			clearInput();
			uint8_t hunter_kill_id = selectOne(30000,R_HUNTER);
			g_playerAlive = g_playerAlive & ~clientIdToBinary(hunter_kill_id);
		}else{
			g_playerGreenLight = 0;
			renderLight();
			delay(3000);
		}
		
	}else if(suspectId == g_moronId){
		Serial.println("Do you want to use your ability?");
		g_playerRedLight = clientIdToBinary(suspectId);
		g_playerGreenLight = clientIdToBinary(suspectId);
		renderLight();
		delay(3000);
		clearInput();
		if(confirm(30000,R_MORON)){
			g_playerRedLight = 0;
			suspectId = 0;
			renderLight();
		}else{
			g_playerGreenLight = 0;
			renderLight();
			
		}
		delay(3000);
	}
	g_playerAlive = g_playerAlive & ~clientIdToBinary(suspectId);

	if(g_sheriffId!=0 && (g_playerAlive & clientIdToBinary(g_sheriffId)) == 0){
		clearInput();
		sheriffSelectNew();
	}

	powerOffAllLight();
	g_playerGreenLight = g_playerAlive;
	Serial.println("Today's survivors are");
	delay(1000);
	renderLight();
	delay(5000);
	checkResult();
	g_isFirstLoop = false;
	powerOffAllLight();

}
void test(){
	
	g_playerAlive = 0;//clientIdToBinary(5) |clientIdToBinary(6) | clientIdToBinary(9) | clientIdToBinary(10);
	g_playerRole[0] = R_LYCAN;
	g_playerRole[1] = R_LYCAN;
	g_playerRole[2] = R_LYCAN;
	g_playerRole[3] = R_LYCAN;

	g_playerRole[4] = R_SEER;
	g_playerRole[5] = R_HUNTER;
	g_playerRole[6] = R_WITCH;
	g_playerRole[7] = R_MORON;

	g_playerRole[8] = R_CITIZEN;
	g_playerRole[9] = R_CITIZEN;
	g_playerRole[10] = R_CITIZEN;
	g_playerRole[11] = R_CITIZEN;
	checkResult();
}

