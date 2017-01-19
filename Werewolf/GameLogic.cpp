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
	say("Debug:Using computer serial port!");
	#endif

	#ifdef DRIVER_TYPE_HARDWARE
	// TO DO:
	// 1. Add driver implementation
	#endif

	say("欢迎使用狼人杀电子法官，版本号："+__VERSION);
	delay(M_TIME);
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
	this->status->badgeLost = false;
	this->tstatus = malloc(sizeof(TurnStatus));

	say("游戏开始");


}
void GameLogic::loop(){
	// init turn status
	this->tstatus->suspectId = 0;
	this->tstatus->lycanKillId = 0;
	this->tstatus->witchPosionId = 0;
	this->tstatus->witchSaved = false;
	this->tstatus->hunterEnableSkill = false;

	this->powerOffAllLight();
	say("天黑请闭眼");
	this->onNight();
	this->powerOffAllLight();
	
	say("天亮请睁眼");
	this->onDay();

	this->status->isFirstLoop = false;
}

void GameLogic::say(String data){
	this->conn->outputString(data);
	unsigned long time = data.length() * SPEECH_SPEED * 0.8f + 800;
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
  	this->say("游戏结束，获胜的一方是");
  	delay(L_TIME);
  	this->say("平局，所有玩家获胜！");
  	this->showIdentity();
  	while(1);
  }

  if(werewolfRemain==0){
  	this->say("游戏结束，获胜的一方是");
  	delay(L_TIME);
  	this->say("好人阵营获胜！");
  	this->showIdentity();
  	while(1);
  }

  if(citizenRemain==0 || godRemain==0){
  	this->say("游戏结束，获胜的一方是");
  	delay(L_TIME);
  	this->say("狼人阵营获胜！");
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
	say("狼人请睁眼");
	if(this->status->isFirstLoop){
		say("狼人请按任意键确认");
		this->roleChangeMore(R_CITIZEN,R_LYCAN,LYCAN_NUMBER);
	}
	delay(S_TIME);
	say("狼人请刀人");
	this->tstatus->lycanKillId = this->selectOneWithAllowRole(30000,R_LYCAN,false);
	say("狼人请闭眼");
}

void GameLogic::witchTurn(){
	say("女巫请睁眼");
	if(this->status->isFirstLoop){
		say("女巫请按任意键确认");
		this->status->witchId = this->roleChangeOnce(R_CITIZEN,R_WITCH);
	}
	delay(S_TIME);

	if(this->isPlayerAlive(this->status->witchId)){

		// use cure
		say("今晚的受害者是");
		uint16_t g = 0;
		uint16_t r = this->status->usedCure ? 0 : this->clientIdToBinary(this->tstatus->lycanKillId);
		this->conn->outputLight(g,r);
		delay(S_TIME);
		this->powerOffAllLight();
		delay(S_TIME);
		say("你要使用解药吗");

		bool ableToUseCure = (!this->status->usedCure) && this->tstatus->lycanKillId == this->status->witchId && this->status->isFirstLoop;

		ableToUseCure = ableToUseCure || (!this->status->usedCure && this->tstatus->lycanKillId > 0);
		
		if(ableToUseCure){
			bool result = this->confirmWithRole(R_WITCH,30000,this->tstatus->lycanKillId);
			if(result){
				this->status->usedCure = true;
				this->tstatus->lycanKillId = 0;
				this->tstatus->witchSaved = true;
			}
		}else{
			delay(L_TIME);
		}

		// use posion
		say("你要使用毒药吗");
		
		if((!this->status->usedPosion) && (!this->tstatus->witchSaved)){
			this->tstatus->witchPosionId = this->selectOneWithAllowRole(30000,R_WITCH,false);
			this->status->usedPosion = this->tstatus->witchPosionId != 0;
		}else{
			delay(L_TIME);
		}
	}else{
		say("今晚的受害者是");
		delay(M_TIME);
		say("你要使用解药吗");
		delay(L_TIME);
		say("你要使用毒药吗");
		delay(L_TIME);
	}


	say("女巫请闭眼");
}

void GameLogic::seerTurn(){
	say("预言家请睁眼");
	if(this->status->isFirstLoop){
		say("预言家请按任意键确认身份");
		this->status->seerId = this->roleChangeOnce(R_CITIZEN,R_SEER);
	}
	delay(S_TIME);
	if(this->isPlayerAlive(this->status->seerId)){
		say("预言家请选择验人");
		Pid id = this->selectOneWithAllowRole(30000,R_SEER,true);
		uint16_t l = this->clientIdToBinary(id);

		say("该玩家为");
		if(this->status->playerRole[id-1] == R_LYCAN)
			this->conn->outputLight(0,l);
		else
			this->conn->outputLight(l,0);
		delay(S_TIME);
		this->powerOffAllLight();

	}else{
		say("预言家请选择验人");
		delay(L_TIME);
		say("该玩家为");
		delay(S_TIME);
	}
	say("预言家请闭眼");
}

void GameLogic::hunterTurn(){
	say("猎人请睁眼");
	if(this->status->isFirstLoop){
		say("猎人请按任意键确认身份");
		this->status->hunterId = this->roleChangeOnce(R_CITIZEN,R_HUNTER);
	}
	

	if(this->isPlayerAlive(this->status->hunterId)){
		say("今晚你的技能状态为");
		uint16_t l = this->clientIdToBinary(this->status->hunterId);
		bool ableToUseAbility = this->status->hunterId == this->tstatus->lycanKillId;

		if(ableToUseAbility)
			this->conn->outputLight(l,0);
		else
			this->conn->outputLight(0,l);
		delay(S_TIME);
		this->powerOffAllLight();
		say("你要使用技能吗");
		if(ableToUseAbility)
			this->tstatus->hunterEnableSkill = this->confirmWithRole(R_HUNTER,30000,this->status->hunterId);
		else
			delay(M_TIME);
	}else{
		say("今晚你的技能状态为");
		delay(S_TIME);
		say("你要使用技能吗");
		delay(L_TIME);
	}

	delay(S_TIME);
	say("猎人请闭眼");
}

void GameLogic::moronTurn(){
	if(this->status->isFirstLoop){
    say("白痴请睁眼");
		say("白痴请确认身份");
		this->status->moronId = this->roleChangeOnce(R_CITIZEN,R_MORON);
    delay(S_TIME);
    say("白痴请闭眼");
	}
}

void GameLogic::sheirffCampagin(){
	say("现在开始警长竞选");
	this->status->sheriffId =  this->confirmOneIdentity(true);
	say(String("新的警长为 ") + this->status->sheriffId + " 号玩家");
	uint16_t l = this->clientIdToBinary(this->status->sheriffId);
	this->conn->outputLight(l,0);
	delay(S_TIME);
	this->powerOffAllLight();
}

void GameLogic::changeSheirff(){
	if((!this->status->badgeLost) && (!this->isPlayerAlive(this->status->sheriffId))){
		
		say("请警长选择继任者");
		Pid newS = this->selectOneWithAllowId(30000,this->status->sheriffId,true);
		if(newS==0 || (!this->isPlayerAlive(newS)) || newS == this->status->sheriffId){
			this->status->badgeLost = true;
			say("警长放弃选择继任者，从此以后没有警长");
			delay(S_TIME);
		}else{
			say(String("新的警长为 ") + this->status->sheriffId + " 号玩家");
			this->status->sheriffId = newS;
			uint16_t l = this->clientIdToBinary(newS);
			this->conn->outputLight(l,0);
			delay(S_TIME);
			this->powerOffAllLight();
		}
	}
}

void GameLogic::hunterSkill(){
	say("请选择带走玩家");
	Pid die = this->selectOneWithAllowId(30000,this->status->hunterId,false);
	this->markPlayerDie(die);
}

void GameLogic::moronSkill(){
	if(this->tstatus->suspectId == this->status->moronId){
		this->tstatus->suspectId = 0;
	}
}

void GameLogic::startSpeech(uint16_t speechList, Pid holderId, unsigned long eachTimeout){
	say("现在开始轮流发言");
	delay(M_TIME);
}

void GameLogic::voteForSuspect(){
	say("现在开始投票");
	this->tstatus->suspectId =  this->confirmOneIdentity(false);
	say(String("") + this->tstatus->suspectId + " 号玩家被投票出局");
}

void GameLogic::reportSuvivor(){
	this->powerOffAllLight();
	say("目前的幸存者为");
	uint16_t l = this->status->playerAlive;
	this->conn->outputLight(l,0);
	delay(M_TIME);
	this->powerOffAllLight();
}

void GameLogic::reportVictim(uint16_t deadList){
	this->powerOffAllLight();
	if(deadList){
		say("昨夜死亡的玩家为");
		uint16_t l = deadList;
		this->conn->outputLight(0,l);
		delay(M_TIME);
		this->powerOffAllLight();
	}else{
		say("昨夜是平安夜，无人死亡");
		delay(S_TIME);
	}
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
	if(this->status->isFirstLoop){
		this->sheirffCampagin();
	}

	uint16_t dlist = 0;
	dlist |= this->clientIdToBinary(this->tstatus->lycanKillId);
	dlist |= this->clientIdToBinary(this->tstatus->witchPosionId);
	this->markPlayerDie(this->tstatus->lycanKillId);
	this->markPlayerDie(this->tstatus->witchPosionId);

	this->reportVictim(dlist);
	this->reportSuvivor();
	if(this->tstatus->hunterEnableSkill){
		this->hunterSkill();
	}
	this->checkResult();
	this->changeSheirff();
	Pid holder = this->isPlayerAlive(this->status->sheriffId) ? this->status->sheriffId : this->nextAlivePlayer(1);
	this->startSpeech(this->status->playerAlive,holder,120000);
	this->voteForSuspect();
	if(this->tstatus->suspectId == this->status->hunterId){
		say("你要使用你的技能吗");
		if(this->confirmWithRole(R_HUNTER,30000,this->status->hunterId))
			this->hunterSkill();
	}
	if(this->tstatus->suspectId == this->status->moronId){
		say("你要使用你的技能吗");
		if(this->confirmWithRole(R_MORON,30000,this->status->moronId))
			this->moronSkill();
	}
	this->markPlayerDie(this->tstatus->suspectId);
	this->checkResult();
	delay(S_TIME);
	this->changeSheirff();
	delay(S_TIME);
	this->reportSuvivor();
	delay(S_TIME);
}

// input/output

Pid GameLogic::confirmOneIdentity(bool usingGreenLight){
	uint8_t id,btn;
	this->powerOffAllLight();
	this->conn->clearBuffer();
	while(1){
		if(this->conn->input(id,btn) && this->isPlayerAlive(id)){
			uint16_t l = clientIdToBinary(id);
			usingGreenLight ? this->conn->outputLight(l,0) : this->conn->outputLight(0,l);
			break;
		}
	}
	delay(S_TIME);
	this->powerOffAllLight();
	return id;
}

Pid GameLogic::roleChangeOnce(role_t from,role_t to){
	uint8_t id,btn;
	this->powerOffAllLight();
	this->conn->clearBuffer();
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
void GameLogic::roleChangeMore(role_t from,role_t to,uint8_t count){
	uint8_t id,btn;
	this->powerOffAllLight();
	uint16_t r = 0;
	this->conn->clearBuffer();
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
}

bool GameLogic::confirmWithId(Pid allow, unsigned long timeout,Pid lightsOn){
	uint8_t id,btn;
	unsigned long start = millis();
	unsigned long current = millis();
	this->powerOffAllLight();

	uint16_t l = this->clientIdToBinary(lightsOn);
	this->conn->outputLight(l,l);
	this->conn->clearBuffer();
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
	this->conn->clearBuffer();
	uint16_t l = this->clientIdToBinary(lightsOn);
	this->conn->outputLight(l,l);
	while(timeout==0 || (current - start) <= timeout){
		current = millis();
		if(this->conn->input(id,btn) && this->isPlayerAlive(id) && (this->status->playerRole[id - 1] == allow || allow == R_ALL)){
			if(btn == 3){
				this->conn->outputLight(l,0);
				delay(S_TIME);
				this->powerOffAllLight();
				return true;
			}else if(btn == 4){
				this->conn->outputLight(0,l);
				delay(S_TIME);
				this->powerOffAllLight();
				return false;
			}
		}
	}
	this->powerOffAllLight();
	return false;
}

Pid GameLogic::selectOneWithAllowRole(unsigned long timeout,role_t allow,bool usingGreenLight = false){
	Pid select = this->nextAlivePlayer(0);
	this->powerOffAllLight();
	unsigned long startTime = millis();
	unsigned long current = millis();
	uint8_t id,btn;
	uint16_t l = this->clientIdToBinary(select);
	if(usingGreenLight)
		this->conn->outputLight(l,0);
	else
		this->conn->outputLight(0,l);
	this->conn->clearBuffer();
	while(timeout == 0 || (current - startTime) < timeout){
		current = millis();
		if(this->conn->input(id,btn)){
			if((this->status->playerRole[id - 1] == allow || allow == R_ALL) && this->isPlayerAlive(id)){
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
	Pid select = this->nextAlivePlayer(0);
	this->powerOffAllLight();
	unsigned long startTime = millis();
	unsigned long current = millis();
	uint8_t id,btn;
	this->conn->clearBuffer();
	uint16_t l = this->clientIdToBinary(select);
	if(usingGreenLight)
		this->conn->outputLight(l,0);
	else
		this->conn->outputLight(0,l);

	while(timeout == 0 || (current - startTime) < timeout){
		current = millis();
		if(this->conn->input(id,btn)){
			if(id == allow){
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
