#include "GameLogic.h"

GameLogic::GameLogic(){
	
}

GameLogic::~GameLogic(){
	free(this->status);
}
void GameLogic::init(){
	#ifdef DRIVER_TYPE_COMPUTER
	ComputerDriver *driver = new ComputerDriver();
	conn = driver;
	say("Using computer serial port!");
	#endif

	#ifdef DRIVER_TYPE_HARDWARE
	// TO DO:
	// 1. Add driver implementation
	#endif
	this->powerOnAllLight();

	this->status = malloc(sizeof(GameStatus));
	this->status->hunterId = 0;
	this->status->witchId = 0;
	this->status->seerId = 0;
	this->status->sheriffId = 0;
	this->status->moronId = 0;
	this->status->isFirstLoop = true;
	this->status->guardId = 0;
	for(int i = 0;i<PLAYER_NUMBER;i++)
		this->status->playerRole[i] = R_CITIZEN;
	this->status->playerAlive = ~0;
	this->status->usedPosion = false;
	this->status->usedCure = false;

	say("Game Started");

	this->tstatus = malloc(sizeof(TurnStatus));
}
void GameLogic::loop(){
	// init turn status
	this->tstatus->suspectId = 0;
	this->tstatus->lycanKillId = 0;
	this->tstatus->witchPosionId = 0;
	this->tstatus->witchSaved = false;
	this->tstatus->hunterEnableSkill = false;

	this->powerOffAllLight();
	say("Night is coming, all close eyes");
	this->onNight();

	this->powerOffAllLight();
	
	say("The night is over, all open eyes");
	this->onDay();

	this->status->isFirstLoop = false;
}

void GameLogic::say(String data){
	this->conn->outputString(data);
	unsigned long time = data.length() * SPEECH_SPEED * 0.6f + 1000;
	delay(time);
}

void GameLogic::checkResult(){
  uint8_t werewolfRemain = 0;
  uint8_t citizenRemain = 0;
  uint8_t godRemain = 0;

  for(int i = 0;i<PLAYER_NUMBER;i++){

  	if(!this->isPlayerAlive(i+1))
  		continue;

  	if(this->status->playerRole[i] == R_LYCAN)
  		werewolfRemain++;
  	else if(this->status->playerRole[i] == R_CITIZEN)
  		citizenRemain++;
  	else
  		godRemain++;
  }

  if(werewolfRemain==0 && (citizenRemain==0 || godRemain==0)){
  	this->say("Game Over, the result is");
  	delay(L_TIME);
  	this->say("Draw, everyone win");
  	this->showIdentity();
  	while(1);
  }

  if(werewolfRemain==0){
  	this->say("Game Over, the result is");
  	delay(L_TIME);
  	this->say("Good Man team win!");
  	this->showIdentity();
  	while(1);
  }

  if(citizenRemain==0 || godRemain==0){
  	this->say("Game Over, the result is");
  	delay(L_TIME);
  	this->say("Werewolf team win!");
  	this->showIdentity();
  	while(1);
  }
  //Serial.println(String("Game is not over! ") + werewolfRemain + " werewolves remain. " + citizenRemain + " citizens remain. " + godRemain + " god remain.");
  
}

Pid GameLogic::previousAlivePlayer(Pid select){
	if(this->status->playerAlive == 0){
		return 0;
	}
	select--;
	if(select <= 0 || select > PLAYER_NUMBER){
		select = PLAYER_NUMBER;  
	}
	while(!this->isPlayerAlive(select)){
		select--;
		if(select <= 0 || select > PLAYER_NUMBER){
			select = PLAYER_NUMBER;  
		}
	}
	return select;
}

Pid GameLogic::nextAlivePlayer(Pid select){
	if(this->status->playerAlive == 0){
		return 0;
	}
	select++;
	if(select > PLAYER_NUMBER){
		select = 1;  
	}
	while(!this->isPlayerAlive(select)){
		select++;
		if(select > PLAYER_NUMBER){
			select = 1;  
		}
	}
	return select;
}

uint16_t GameLogic::clientIdToBinary(Pid id){

	if(id > 0)
		return 1 << (id - 1);
	else
		return 0;
}

bool GameLogic::isPlayerAlive(Pid id){
	return (this->status->playerAlive & this->clientIdToBinary(id)) > 0;
}

void GameLogic::markPlayerDie(Pid id){
	this->status->playerAlive &= ~this->clientIdToBinary(id);
}

void GameLogic::markPlayerAlive(Pid id){
	this->status->playerAlive |= this->clientIdToBinary(id);
}

// logic proc
void GameLogic::lycanTurn(){
	say("Werewolf open eyes");
	if(this->status->isFirstLoop){
		say("Werewolf confirm identity");
		this->roleChangeMore(R_CITIZEN,R_LYCAN,LYCAN_NUMBER);
	}
	delay(S_TIME);
	say("Werewolf pick someone to kill");
	this->tstatus->lycanKillId = this->selectOneWithAllowRole(30000,R_LYCAN,false);
	say("Werewolf close eyes");
}

void GameLogic::witchTurn(){
	say("Witch open eyes");
	
	say("Witch close eyes");
}

void GameLogic::seerTurn(){
	say("Seer open eyes");

	say("Seer close eyes");
}

void GameLogic::hunterTurn(){
	say("Hunter open eyes");

	say("Hunter close eyes");
}

void GameLogic::moronTurn(){
	say("Moron open eyes");

	say("moron close eyes");
}

void GameLogic::onNight(){
	this->lycanTurn();
	delay(S_TIME);
	this->witchTurn();
	delay(S_TIME);
	this->seerTurn();
	delay(S_TIME);
	this->hunterTurn();
	delay(S_TIME);
	this->moronTurn();
	delay(S_TIME);
}

void GameLogic::onDay(){

}

// input/output
Pid GameLogic::roleChangeOnce(role_t from,role_t to){
	uint8_t id,btn;
	this->powerOffAllLight();
	while(1){
		if(this->conn->input(id,btn) && this->isPlayerAlive(id) && this->status->playerRole[id-1] == from){
			this->status->playerRole[id-1] = to;
			uint16_t g = clientIdToBinary(id);
			this->conn->outputLight(g,0);
			break;
		}
	}

	delay(S_TIME);
	this->powerOffAllLight();
	return id;
}
Pid GameLogic::roleChangeMore(role_t from,role_t to,uint8_t count){
	uint8_t id,btn;
	this->powerOffAllLight();
	uint16_t r = 0;
	while(count > 0){
		if(this->conn->input(id,btn) && this->isPlayerAlive(id) && this->status->playerRole[id-1] == from){
			this->status->playerRole[id-1] = to;
			r |= this->clientIdToBinary(id);
			this->conn->outputLight(0,r);
			count--;
		}
	}
	delay(S_TIME);
	this->powerOffAllLight();
	return id;
}

bool GameLogic::confirmWithId(Pid allow, unsigned long timeout,Pid lightsOn){
	uint8_t id,btn;
	unsigned long start = millis();
	unsigned long current = millis();
	this->powerOffAllLight();

	uint16_t l = this->clientIdToBinary(lightsOn);
	this->conn->outputLight(l,l);
	while(timeout==0 || (current - start) <= timeout){
		current = millis();
		if(this->conn->input(id,btn) && this->isPlayerAlive(id) && id == allow){
			if(btn == 3){
				this->conn->outputLight(l,0);
				delay(S_TIME);
				return true;
			}else if(btn == 4){
				this->conn->outputLight(0,l);
				delay(S_TIME);
				return false;
			}
		}
	}
	this->powerOffAllLight();
	return false;
}

bool GameLogic::confirmWithRole(role_t allow, unsigned long timeout,Pid lightsOn){
	uint8_t id,btn;
	unsigned long start = millis();
	unsigned long current = millis();
	this->powerOffAllLight();
	
	uint16_t l = this->clientIdToBinary(lightsOn);
	this->conn->outputLight(l,l);
	while(timeout==0 || (current - start) <= timeout){
		current = millis();
		if(this->conn->input(id,btn) && this->isPlayerAlive(id) && this->status->playerRole[id-1] == allow){
			if(btn == 3){
				this->conn->outputLight(l,0);
				delay(S_TIME);
				return true;
			}else if(btn == 4){
				this->conn->outputLight(0,l);
				delay(S_TIME);
				return false;
			}
		}
	}
	this->powerOffAllLight();
	return false;
}

Pid GameLogic::selectOneWithAllowRole(unsigned long timeout,role_t allow,bool usingGreenLight = false){
	Pid select = this->nextAlivePlayer(1);
	this->powerOffAllLight();
	unsigned long startTime = millis();
	unsigned long current = millis();
	uint8_t id,btn;
	uint16_t l = this->clientIdToBinary(select);
	if(usingGreenLight)
		this->conn->outputLight(l,0);
	else
		this->conn->outputLight(0,l);

	while(timeout == 0 || (current - startTime) < timeout){
		current = millis();
		if(this->conn->input(id,btn)){
			if(this->status->playerRole[id - 1] == allow && this->isPlayerAlive(id)){
				if(btn == 3){
					delay(XS_TIME);
					this->powerOffAllLight();
					return select;
				}else if(btn==1){
					select = this->previousAlivePlayer(select);
					l = this->clientIdToBinary(select);
					if(usingGreenLight)
						this->conn->outputLight(l,0);
					else
						this->conn->outputLight(0,l);
				}else if(btn==2){
					select = nextAlivePlayer(select);
					l = this->clientIdToBinary(select);
					if(usingGreenLight)
						this->conn->outputLight(l,0);
					else
						this->conn->outputLight(0,l);
				}else if(btn==4){
					delay(XS_TIME);
					this->powerOffAllLight();
					return 0;
				}
			}
		}
	}
	this->powerOffAllLight();
	return 0;
}

Pid GameLogic::selectOneWithAllowId(unsigned long timeout,Pid allow,bool usingGreenLight = false){
	Pid select = this->nextAlivePlayer(1);
	this->powerOffAllLight();
	unsigned long startTime = millis();
	unsigned long current = millis();
	uint8_t id,btn;
	uint16_t l = this->clientIdToBinary(select);
	if(usingGreenLight)
		this->conn->outputLight(l,0);
	else
		this->conn->outputLight(0,l);

	while(timeout == 0 || (current - startTime) < timeout){
		current = millis();
		if(this->conn->input(id,btn)){
			if(this->status->playerRole[id - 1] == allow && this->isPlayerAlive(id)){
				if(btn == 3){
					delay(XS_TIME);
					this->powerOffAllLight();
					return select;
				}else if(btn==1){
					select = this->previousAlivePlayer(select);
					l = this->clientIdToBinary(select);
					if(usingGreenLight)
						this->conn->outputLight(l,0);
					else
						this->conn->outputLight(0,l);
				}else if(btn==2){
					select = nextAlivePlayer(select);
					l = this->clientIdToBinary(select);
					if(usingGreenLight)
						this->conn->outputLight(l,0);
					else
						this->conn->outputLight(0,l);
				}else if(btn==4){
					delay(XS_TIME);
					this->powerOffAllLight();
					return 0;
				}
			}
		}
	}
	this->powerOffAllLight();
	return 0;
}

// light control
void GameLogic::powerOffAllLight(){
	this->conn->outputLight(0,0);
}

void GameLogic::powerOnAllLight(){
	this->conn->outputLight(~0,~0);
}

void GameLogic::showIdentity(){
	this->powerOffAllLight();
	uint16_t red = 0;
	uint16_t green = 0;

	for(int i = 0;i<PLAYER_NUMBER;i++){
		if(this->status->playerRole[i] == R_LYCAN)
			red |= this->clientIdToBinary(i+1);
		else
			green |= this->clientIdToBinary(i+1);
	}
	this->conn->outputLight(green,red);
}