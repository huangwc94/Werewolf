#include "GameLogic.h"
#include "LycanSusideIndicator.h"

GameLogic::GameLogic(){

}

GameLogic::~GameLogic(){
	free(this->status);
}
void GameLogic::init(){

	#ifdef DRIVER_TYPE_HARDWARE
	HardwareDriver *driver = new HardwareDriver(PLAYER_NUMBER);
	conn = driver;
	say("Debug:Using HardwareDriver");

	#else

	ComputerDriver *driver = new ComputerDriver();
	conn = driver;
	say("Debug:Using computer serial port!");

	#endif



	delay(100);
	this->powerOffAllLight();
	this->conn->playSound(1);
	say("正在检测手柄状态");
	this->checkClient();
	this->conn->playSound(2);
	this->powerOffAllLight();
	say(String("欢迎使用狼人杀电子法官"));
	delay(S_TIME);

	// Instruction
	// this->conn->playSound(67);
	// uint8_t id,btn;
	// uint16_t r = 0;
	// uint8_t count = PLAYER_NUMBER;
	// uint16_t skip = 0;
	// this->conn->clearBuffer();
	// unsigned long start = millis();
	// while(count > 0 && (millis() - start) < 52000){
	// 	if(this->conn->input(id,btn) && (skip & this->clientIdToBinary(id)) == 0){
	// 		skip |= this->clientIdToBinary(id);
	// 		r |= this->clientIdToBinary(id);
	// 		this->conn->outputLight(0,r);
	// 		count--;
	// 	}
	// 	this->conn->setScreen((start + 52000 -millis())/1000 + 1);
	// }
	// this->conn->setScreen(10000);
	//
	//
	this->powerOffAllLight();
	this->conn->playSound(3);
	say("游戏开始");



	this->status = (GameStatus*)malloc(sizeof(GameStatus));
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
	this->status->yesterdayDead = 0;
	this->status->autoSeerSheirrf = false;
	this->status->seerChecked = 0;
	this->status->moronUsed = false;
	this->tstatus = (TurnStatus*)malloc(sizeof(TurnStatus));



}
void GameLogic::loop(){
	// init turn status
	this->tstatus->suspectId = 0;
	this->tstatus->lycanKillId = 0;
	this->tstatus->witchPosionId = 0;
	this->tstatus->witchSaved = false;
	this->tstatus->lycanSusideId = 0;
	this->powerOffAllLight();

	this->conn->playSound(4);
	say("天黑请闭眼");
	delay(L_TIME);
	this->onNight();

	this->checkResult_before_day();

	this->powerOffAllLight();
	this->conn->playSound(5);
	say("天亮请睁眼");
	this->onDay();
	this->checkResult_before_night();

	this->status->isFirstLoop = false;
}

void GameLogic::say(String data){
	this->conn->outputString(data);
	unsigned long time = data.length() * SPEECH_SPEED * 0.8f + 800;
	delay(time);
}
void GameLogic::checkResult_before_day(){

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
	this->old_check_result(werewolfRemain,citizenRemain,godRemain);

	return;

	if(this->tstatus->lycanKillId>0){
		if(this->status->playerRole[this->tstatus->lycanKillId] == R_LYCAN){
			werewolfRemain--;
		}else if(this->status->playerRole[this->tstatus->lycanKillId] == R_CITIZEN){
			citizenRemain--;
		}else{
			godRemain--;
		}
	}
	if(this->tstatus->witchPosionId > 0){
		if(this->status->playerRole[this->tstatus->witchPosionId] == R_LYCAN){
			werewolfRemain--;
		}else if(this->status->playerRole[this->tstatus->witchPosionId] == R_CITIZEN){
			citizenRemain--;
		}else{
			godRemain--;
		}
	}


	bool werewolfWin = false;
	if(!this->status->badgeLost && this->status->playerRole[this->status->sheriffId] == R_LYCAN){
		if(werewolfRemain >= (citizenRemain + godRemain)){
			werewolfWin = true;
		}

	}else{
		if(werewolfRemain == (citizenRemain + godRemain + 1)){
			werewolfWin = true;
		}
	}

	uint8_t werewolfCheckCounter = 0;
	if(this->status->autoSeerSheirrf){
		for(Pid i = 1 ;i < PLAYER_NUMBER;i++){
			if((this->status->seerChecked & this->clientIdToBinary(i)) == 0) continue;
			if(this->status->playerRole[i - 1] == R_LYCAN){
				werewolfCheckCounter++;
			}
		}
	}

	bool goodManWin = werewolfCheckCounter == LYCAN_NUMBER;

	if(werewolfWin){
			this->conn->playSound(6);
			this->say("游戏结束，获胜的一方是");
			delay(M_TIME);
			this->conn->playSound(9);
			this->say("狼人阵营获胜！");
			this->showIdentity();
			while(1);
	}
	if(goodManWin){
		this->conn->playSound(6);
		this->say("游戏结束，获胜的一方是");
		delay(M_TIME);
		this->conn->playSound(8);
		this->say("好人阵营获胜！");
		this->showIdentity();
		while(1);
	}
}
void GameLogic::old_check_result(uint8_t werewolfRemain,uint8_t citizenRemain,uint8_t godRemain){

  if(werewolfRemain==0 && (citizenRemain==0 || godRemain==0)){
		this->conn->playSound(6);
  	this->say("游戏结束，获胜的一方是");
  	delay(M_TIME);
		this->conn->playSound(7);
  	this->say("平局，所有玩家获胜！");
  	this->showIdentity();
  	while(1);
  }

  if(werewolfRemain==0){
		this->conn->playSound(6);
  	this->say("游戏结束，获胜的一方是");
  	delay(M_TIME);
		this->conn->playSound(8);
  	this->say("好人阵营获胜！");
  	this->showIdentity();
  	while(1);
  }

  if(citizenRemain==0 || godRemain==0){
		this->conn->playSound(6);
  	this->say("游戏结束，获胜的一方是");
  	delay(M_TIME);
		this->conn->playSound(9);
  	this->say("狼人阵营获胜！");
  	this->showIdentity();
  	while(1);
  }
}
void GameLogic::checkResult_before_night(){

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

	this->old_check_result(werewolfRemain,citizenRemain,godRemain);
	return;
	
	if(this->isPlayerAlive(this->status->moronId) && this->status->moronUsed)
		godRemain--;

	uint8_t goodManRemain = godRemain + citizenRemain;
	bool werewolfWin = false;


	//狼警长，狼人与好人一样多
	werewolfWin = werewolfWin || (!this->status->badgeLost
									&& (this->status->playerRole[this->status->sheriffId - 1] == R_LYCAN)
									&& this->isPlayerAlive(this->status->sheriffId)
									&& (werewolfRemain >= goodManRemain));
	// 狼警长，狼人 = 好人 - 1 ，并且女巫双药全无，并且无猎人
  werewolfWin = werewolfWin || (!this->status->badgeLost
									&& (this->status->playerRole[this->status->sheriffId - 1] == R_LYCAN)
									&& (werewolfRemain == (goodManRemain - 1))
									&& this->status->usedPosion && this->status->usedCure
									&& (!this->isPlayerAlive(this->status->hunterId))
	);

	// 非狼警长, 狼人比好人多
	werewolfWin = werewolfWin || (
		(this->status->badgeLost || this->status->playerRole[this->status->sheriffId - 1] != R_LYCAN)
		&& (werewolfRemain > goodManRemain)
	);
	// 非狼警长，狼人 = 好人  ，并且女巫双药全无，并且无猎人
	werewolfWin = werewolfWin || (
									(this->status->badgeLost || this->status->playerRole[this->status->sheriffId - 1] != R_LYCAN)
									&& (werewolfRemain > (godRemain + citizenRemain))
									&& (werewolfRemain == goodManRemain)
									&& this->status->usedPosion && this->status->usedCure
									&& (!this->isPlayerAlive(this->status->hunterId))
	);

	if(werewolfWin){
			this->conn->playSound(6);
			this->say("游戏结束，获胜的一方是");
			delay(M_TIME);
			this->conn->playSound(9);
			this->say("狼人阵营获胜！");
			this->showIdentity();
			while(1);
	}
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
	this->status->yesterdayDead = id;
	this->status->playerAlive &= ~this->clientIdToBinary(id);
}

void GameLogic::markPlayerAlive(Pid id){
	this->status->playerAlive |= this->clientIdToBinary(id);
}

// logic proc
void GameLogic::lycanTurn(){
	this->conn->playSound(10);
	say("狼人请睁眼");
	if(this->status->isFirstLoop){
		this->conn->playSound(11);
		say("狼人请按任意键确认");
		this->roleChangeMore(R_CITIZEN,R_LYCAN,LYCAN_NUMBER);
	}
	delay(S_TIME);
	this->conn->playSound(12);
	say("狼人请刀人");
	this->tstatus->lycanKillId = this->selectOneWithAllowRole(SELECT_TIME,R_LYCAN,false);
	this->conn->playSound(13);
	say("狼人请闭眼");
}

void GameLogic::witchTurn(){
	this->conn->playSound(14);
	say("女巫请睁眼");
	if(this->status->isFirstLoop){
		this->conn->playSound(15);
		say("女巫请按任意键确认");
		this->status->witchId = this->roleChangeOnce(R_CITIZEN,R_WITCH);
	}
	delay(S_TIME);

	if(this->isPlayerAlive(this->status->witchId)){

		// use cure
		this->conn->playSound(16);
		say("今晚的受害者是");
		uint16_t g = 0;
		uint16_t r = this->status->usedCure ? 0 : this->clientIdToBinary(this->tstatus->lycanKillId);
		this->conn->outputLight(g,r);
		delay(S_TIME);
		this->powerOffAllLight();
		delay(S_TIME);
		this->conn->playSound(17);
		say("你要使用解药吗");

		bool ableToUseCure = true;
		if(!this->status->isFirstLoop){
			ableToUseCure = !this->status->usedCure && this->tstatus->lycanKillId > 0 && this->tstatus->lycanKillId != this->status->witchId;
		}

		if(ableToUseCure){


			bool result = this->confirmWithRole(R_WITCH,CONFIRM_TIME,this->tstatus->lycanKillId);
			if(result){
				this->status->usedCure = true;
				this->tstatus->lycanKillId = 0;
				this->tstatus->witchSaved = true;
			}
		}else{
			delay(L_TIME);
		}

		// use posion
		this->conn->playSound(18);
		say("你要使用毒药吗");

		if((!this->status->usedPosion) && (!this->tstatus->witchSaved)){
			bool result = this->confirmWithRole(R_WITCH,CONFIRM_TIME,this->status->witchId);
			if(result){
				this->tstatus->witchPosionId = this->selectOneWithAllowRole(SELECT_TIME,R_WITCH,false);
				this->status->usedPosion = this->tstatus->witchPosionId != 0;
			}
		}else{
			delay(L_TIME);
		}
	}else{
		this->conn->playSound(16);
		say("今晚的受害者是");
		delay(M_TIME);
		this->conn->playSound(17);
		say("你要使用解药吗");
		delay(L_TIME);
		this->conn->playSound(18);
		say("你要使用毒药吗");
		delay(L_TIME);
	}

	this->conn->playSound(19);
	say("女巫请闭眼");
}

void GameLogic::seerTurn(){
	this->conn->playSound(20);
	say("预言家请睁眼");
	if(this->status->isFirstLoop){
		this->conn->playSound(21);
		say("预言家请按任意键确认");
		this->status->seerId = this->roleChangeOnce(R_CITIZEN,R_SEER);
	}
	delay(S_TIME);
	if(this->isPlayerAlive(this->status->seerId)){
		this->conn->playSound(22);
		say("预言家请选择验人");
		Pid id = this->selectOneWithAllowRole(SELECT_TIME,R_SEER,true);
		uint16_t l = this->clientIdToBinary(id);
		this->conn->playSound(23);
		say("该玩家为");
		this->status->seerChecked |= this->clientIdToBinary(id);
		if(this->status->playerRole[id-1] == R_LYCAN){
			this->conn->outputLight(0,l);

		}else{
			this->conn->outputLight(l,0);

		}

		delay(S_TIME);
		this->powerOffAllLight();

	}else{
		this->conn->playSound(22);
		say("预言家请选择验人");
		delay(L_TIME);
		this->conn->playSound(23);
		say("该玩家为");
		delay(S_TIME);
	}
	this->conn->playSound(24);
	say("预言家请闭眼");
}

void GameLogic::hunterTurn(){
	this->conn->playSound(25);
	say("猎人请睁眼");
	if(this->status->isFirstLoop){
		this->conn->playSound(26);
		say("猎人请按任意键确认身份");
		this->status->hunterId = this->roleChangeOnce(R_CITIZEN,R_HUNTER);
	}


	if(this->isPlayerAlive(this->status->hunterId)){
		this->conn->playSound(27);
		say("今晚你的技能状态为");
		uint16_t l = this->clientIdToBinary(this->status->hunterId);
		bool ableToUseAbility = this->status->hunterId != this->tstatus->witchPosionId;

		if(ableToUseAbility)
			this->conn->outputLight(l,0);
		else
			this->conn->outputLight(0,l);
		delay(S_TIME);
		this->powerOffAllLight();
		delay(XS_TIME);
	}else{
		this->conn->playSound(27);
		say("今晚你的技能状态为");
		delay(M_TIME);

	}

	delay(S_TIME);
	this->conn->playSound(29);
	say("猎人请闭眼");
}

void GameLogic::moronTurn(){
	if(this->status->isFirstLoop){
		this->conn->playSound(30);
    say("白痴请睁眼");
		this->conn->playSound(31);
		say("白痴请确认身份");
		this->status->moronId = this->roleChangeOnce(R_CITIZEN,R_MORON);
    delay(S_TIME);
		this->conn->playSound(32);
    say("白痴请闭眼");
	}
}

void GameLogic::sheirffCampagin(){
	this->powerOffAllLight();
	this->conn->playSound(33);
	say("现在开始警长竞选，请在3秒内按确认键参与竞选");
	uint16_t candidate = 0;
	uint8_t btn,id;
	uint8_t counter = 0;
	Timer timer(3000,this->conn);
	this->conn->clearBuffer();
	unsigned long start = millis();
	while(millis() - start < 5000){
		timer.run();
		if(this->conn->input(id,btn) && this->isPlayerAlive(id)){
			if(btn == 3 || btn == 5){
				candidate |= this->clientIdToBinary(id);
				counter ++;
				this->conn->outputLight(0, candidate);
			}
		}
	}
	this->conn->setScreen(10000);
	if(candidate == 0){
		this->status->badgeLost = true;
		this->status->sheriffId = 0;
		this->conn->playSound(68);
		say("没有人参与竞选警长，从此以后没有警长。");
		return;
	}

	Pid sheriff = 0;
	if(counter > 1){
		this->conn->playSound(61);
		LycanSusideIndicator lsi(this);
		say("现在开始竞选发言，请按确认键结束发言，取消键退出竞选");
		unsigned long current;
		bool lightup = true;
		unsigned long random = millis();
		random = random % 48;
		for(uint16_t q = (int)random; q<(PLAYER_NUMBER + random) && counter > 1; q++){
			Pid i = (q % PLAYER_NUMBER) + 1;

			if((candidate & this->clientIdToBinary(i)) == 0) continue;

			uint16_t light = this->clientIdToBinary(i);
			this->conn->outputLight(light, candidate);
			this->conn->playSound(34 + i);
			say(String(i) + "号");
			this->conn->playSound(56);
			say("发言");
			lightup = true;
			Timer timer(SPEECH_TIME,this->conn);
			this->conn->clearBuffer();
			while(timer.run()){

				// button control
				if(this->conn->input(id,btn)){

					if(lsi.detect(id, btn)){
						this->status->badgeLost = true;
						return;
					}

					if(!this->isPlayerAlive(i)){
						candidate = candidate & ~ this->clientIdToBinary(i);
						break;
					}

					if((candidate & this->clientIdToBinary(id)) == 0){
						continue;
					}

					if(id == i && (btn == 5 || btn == 3)){
						break;
					}
					if(btn == 4){
						candidate &= ~ this->clientIdToBinary(id);
						if(lightup)
							this->conn->outputLight(light, candidate);
						else
							this->conn->outputLight(0, candidate);
						counter--;
						if(counter < 2){
							break;
						}
						if(id == i){
							break;
						}
					}
				}

				// blink
				if(millis() - current >= 500){
					current = millis();
					if(lightup)
						this->conn->outputLight(light, candidate);
					else
						this->conn->outputLight(0, candidate);
					lightup = !lightup;
				}
			}
			this->conn->outputLight(0, candidate);
		}
		this->conn->playSound(65);
		say("发言完毕");
	}

	for(int i = 1;i<=PLAYER_NUMBER;i++){
		if((candidate & this->clientIdToBinary(i)) > 0){
			if(sheriff == 0){
				sheriff = i;
			}else{
				sheriff = 0;
				break;
			}
		}
	}


	if(sheriff == 0){
		this->conn->playSound(64);
		this->conn->outputLight(0, candidate);
		say("请新警长按任意键确认");
		this->conn->clearBuffer();
		while(1){
			if(this->conn->input(id,btn)){
				if(candidate == 0 || (candidate & this->clientIdToBinary(id)) > 0){
					uint16_t l = clientIdToBinary(id);
					this->conn->outputLight(l,0);
					sheriff = id;
					delay(S_TIME);
					break;
				}
			}
		}
	}else if(sheriff == this->status->seerId){
		this->status->autoSeerSheirrf = true;
	}


	this->status->sheriffId =  sheriff;

	this->powerOffAllLight();
	// new sheriff shows
	this->conn->playSound(34);
	say(String("新的警长为"));
	this->conn->playSound(34 + this->status->sheriffId);
	uint16_t l = this->clientIdToBinary(this->status->sheriffId);
	this->conn->outputLight(l,l);
	say(String(this->status->sheriffId) + "号玩家");

	delay(S_TIME);
	this->powerOffAllLight();
}

void GameLogic::changeSheirff(){
	if((!this->status->badgeLost) && (!this->isPlayerAlive(this->status->sheriffId))){
		this->conn->playSound(47);
		say("请警长选择继任者,取消，超时或选择已亮身份的白痴则视为放弃");
		Pid newS = this->selectOneWithAllowId(SELECT_TIME,this->status->sheriffId,true);
		if(newS==0
			|| (!this->isPlayerAlive(newS))
			|| newS == this->status->sheriffId
			|| (this->status->moronUsed && newS == this->status->moronId)){
			this->status->badgeLost = true;
			this->conn->playSound(48);
			say("警长放弃选择继任者，从此以后没有警长");
			delay(S_TIME);
		}else{
			this->status->sheriffId = newS;
			this->conn->playSound(34);
			say(String("新的警长为"));
			this->conn->playSound(34 + this->status->sheriffId);
			uint16_t l = this->clientIdToBinary(newS);
			this->conn->outputLight(l,0);
			say(String(this->status->sheriffId) + "号玩家");
			delay(S_TIME);
			this->powerOffAllLight();
		}
	}
}

void GameLogic::hunterSkill(){
	this->conn->playSound(49);
	say("请选择带走玩家");
	Pid die = this->selectOneWithAllowId(0,this->status->hunterId,false);

	if(this->status->isFirstLoop && this->tstatus->lycanSusideId==0){
		this->playerSpeech(die);
		this->powerOffAllLight();
	}
	this->markPlayerDie(die);
}

void GameLogic::playerSpeech(Pid speecher){
	if(this->tstatus->lycanSusideId != 0 ) return;
	uint16_t light = this->clientIdToBinary(speecher);
	this->conn->outputLight(this->status->playerAlive, light);
	this->conn->playSound(34 + speecher);
	say(String(speecher) + "号");
	this->conn->playSound(56);
	say("发言");
	bool lightup = true;
	unsigned long current = millis();
	uint8_t btn,id;
	Timer timer(SPEECH_TIME,this->conn);
	this->conn->clearBuffer();
	LycanSusideIndicator lsi(this);
	while(timer.run() && this->isPlayerAlive(speecher)){
		if(this->conn->input(id,btn)){
			if(lsi.detect(id, btn)){
				return;
			}
			if(id == speecher && (btn == 5 || btn == 3)){
				break;
			}

		}
		if(millis() - current >= 500){
			current = millis();
			if(lightup)
				this->conn->outputLight(this->status->playerAlive, light);
			else
				this->conn->outputLight(this->status->playerAlive, 0);
			lightup = !lightup;
		}
	}
	this->conn->outputLight(this->status->playerAlive, 0);
	delay(S_TIME);
}

void GameLogic::moronSkill(){
	if(this->tstatus->suspectId == this->status->moronId){
		this->tstatus->suspectId = 0;
		this->status->moronUsed = true;

		this->markPlayerDie(this->status->moronId);
		this->changeSheirff();
		this->markPlayerAlive(this->status->moronId);
	}
}

void GameLogic::startSpeech(){
	this->conn->playSound(50);
	say("现在开始轮流发言");
	uint8_t id,btn;
	bool fromLeft = true;
	Pid startId = this->status->yesterdayDead;
	this->conn->outputLight(this->status->playerAlive, 0);
	if(!this->status->badgeLost && this->isPlayerAlive(this->status->sheriffId)){
		this->conn->playSound(57);
		startId = this->status->sheriffId;
		say("请警长选择从左或从右开始发言");
		this->conn->clearBuffer();
		while(1){
			if(this->conn->input(id,btn) && id == this->status->sheriffId){
				if(btn == 1){
					fromLeft = true;
					break;
				}else if(btn == 2){
					fromLeft = false;
					break;
				}
			}
		}
	}
	Pid currentSpeechPlayer = fromLeft ? this->previousAlivePlayer(startId) : this->nextAlivePlayer(startId) ;
	startId = currentSpeechPlayer;

	do{
		if((!this->status->moronUsed) || currentSpeechPlayer != this->status->moronId){
			this->playerSpeech(currentSpeechPlayer);
		}

		if(this->tstatus->lycanSusideId > 0) return;
		if(fromLeft){
			currentSpeechPlayer = this->previousAlivePlayer(currentSpeechPlayer);

		}else{
			currentSpeechPlayer = this->nextAlivePlayer(currentSpeechPlayer);
		}

	}while(startId != currentSpeechPlayer && this->tstatus->lycanSusideId == 0);
	this->conn->playSound(65);
	say("发言完毕");
}

void GameLogic::voteForSuspect(){
	this->conn->outputLight(this->status->playerAlive, 0);
	this->conn->playSound(51);
	say("现在开始投票");
	this->tstatus->suspectId =  this->confirmOneIdentity(false);
	this->conn->playSound(34 + this->tstatus->suspectId);
	say(String(this->tstatus->suspectId) + "号玩家");
	this->conn->playSound(52);
	say("被投票出局");

	if(this->tstatus->suspectId == this->status->moronId){
		this->conn->playSound(28);
		say("你要使用技能吗");
		if(this->confirmWithRole(R_MORON,CONFIRM_TIME,this->status->moronId))
			this->moronSkill();
		this->playerSpeech(this->status->moronId);
	}

	if(this->tstatus->suspectId!=0){
		this->playerSpeech(this->tstatus->suspectId);
	}

	this->powerOffAllLight();
	this->conn->playSound(65);
	say("发言完毕");
}

void GameLogic::reportSuvivor(){
	this->powerOffAllLight();
	this->conn->playSound(53);
	say("目前的幸存者为");
	uint16_t l = this->status->playerAlive;
	this->conn->outputLight(l,0);
	delay(M_TIME);
	this->powerOffAllLight();
}

void GameLogic::reportVictim(uint16_t deadList){
	this->powerOffAllLight();
	if(deadList){
		this->conn->playSound(54);
		say("昨夜死亡的玩家为");
		uint16_t l = deadList;
		this->conn->outputLight(0,l);
		delay(M_TIME);
		this->powerOffAllLight();
	}else{
		this->conn->playSound(55);
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


	this->reportVictim(dlist);

	if(this->status->isFirstLoop && this->tstatus->lycanSusideId == 0){
		for(Pid i = 1;i<PLAYER_NUMBER && this->tstatus->lycanSusideId == 0;i++){
			if((dlist & this->clientIdToBinary(i)) == 0) continue;
			this->playerSpeech(i);
		}
		this->powerOffAllLight();
	}

	if(this->status->hunterId != this->tstatus->witchPosionId
			&& this->tstatus->lycanKillId == this->status->hunterId
			&& this->isPlayerAlive(this->status->hunterId)){
		this->conn->playSound(28);
		say("你要使用技能吗");
		if(this->confirmWithRole(R_HUNTER,CONFIRM_TIME,this->status->hunterId))
			this->hunterSkill();
	}

	this->markPlayerDie(this->tstatus->lycanKillId);
	this->markPlayerDie(this->tstatus->witchPosionId);
	if(this->tstatus->lycanSusideId > 0){
		this->markPlayerDie(this->tstatus->lycanSusideId);
		this->changeSheirff();
		delay(S_TIME);
		this->reportSuvivor();
		delay(S_TIME);
		return;
	}

	this->changeSheirff();
	this->startSpeech();

	if(this->tstatus->lycanSusideId > 0){
		this->markPlayerDie(this->tstatus->lycanSusideId);
		this->changeSheirff();
		delay(S_TIME);
		this->reportSuvivor();
		delay(S_TIME);
		return;
	}

	this->voteForSuspect();
	if(this->tstatus->suspectId == this->status->hunterId && this->isPlayerAlive(this->status->hunterId)){
		this->conn->playSound(28);
		say("你要使用技能吗");

		if(this->confirmWithRole(R_HUNTER,CONFIRM_TIME,this->status->hunterId))
			this->hunterSkill();
	}
	this->markPlayerDie(this->tstatus->suspectId);
	this->markPlayerDie(this->tstatus->lycanSusideId);
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
	this->powerOffAllLight();

	uint16_t l = this->clientIdToBinary(lightsOn);
	this->conn->outputLight(l,l);

	this->conn->clearBuffer();
	Timer timer(timeout,this->conn);
	while(timer.run()){
		if(this->conn->input(id,btn) && this->isPlayerAlive(id) && id == allow){
			if(btn == 3 || btn == 5){
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
	this->powerOffAllLight();
	this->conn->clearBuffer();
	uint16_t l = this->clientIdToBinary(lightsOn);
	this->conn->outputLight(l,l);

	Timer timer(timeout,this->conn);
	while(timer.run()){
		if(this->conn->input(id,btn) && this->isPlayerAlive(id) && (this->status->playerRole[id - 1] == allow || allow == R_ALL)){
			if(btn == 3 || btn == 5){
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

	uint8_t id,btn;
	uint16_t l = this->clientIdToBinary(select);
	if(usingGreenLight)
		this->conn->outputLight(l,0);
	else
		this->conn->outputLight(0,l);
	this->conn->clearBuffer();

	Timer timer(timeout,this->conn);

	while(timer.run()){
		if(this->conn->input(id,btn)){
			if((this->status->playerRole[id - 1] == allow || allow == R_ALL) && this->isPlayerAlive(id)){
				if(btn == 3 || btn == 5){
					if(usingGreenLight){
						delay(S_TIME);
						this->powerOffAllLight();
						return select;
					}
					this->powerOffAllLight();
					delay(500);
					this->conn->outputLight(0,l);
					delay(500);
					this->powerOffAllLight();
					delay(500);
					this->conn->outputLight(0,l);
					delay(500);
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

	uint8_t id,btn;
	this->conn->clearBuffer();
	uint16_t l = this->clientIdToBinary(select);
	if(usingGreenLight)
		this->conn->outputLight(l,0);
	else
		this->conn->outputLight(0,l);

	Timer timer(timeout,this->conn);

	while(timer.run()){
		if(this->conn->input(id,btn)){
			if(id == allow){
				if(btn == 3 || btn == 5){
					if(usingGreenLight){
						delay(S_TIME);
						this->powerOffAllLight();
						return select;
					}
					this->powerOffAllLight();
					delay(500);
					this->conn->outputLight(0,l);
					delay(500);
					this->powerOffAllLight();
					delay(500);
					this->conn->outputLight(0,l);
					delay(500);
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
		else if(this->status->playerRole[i] == R_CITIZEN)
			green |= this->clientIdToBinary(i+1);
		else{
			red |= this->clientIdToBinary(i+1);
			green |= this->clientIdToBinary(i+1);
		}
	}
	this->conn->outputLight(green,red);
}

void GameLogic::checkClient(){
	this->powerOnAllLight();
	delay(1000);
	this->powerOffAllLight();
	delay(1000);
	uint16_t r = 0;
	uint16_t g = 0;
	for(int i = 1;i<=PLAYER_NUMBER;i++){

		for(int j = i ; j <= PLAYER_NUMBER - i + 1;j++){
			int q = PLAYER_NUMBER - j + 1;
			this->conn->setScreen(i);
			uint16_t l1 = this->clientIdToBinary(q);
			uint16_t l2 = this->clientIdToBinary(j);
			g |= l1;
			r |= l2;
			this->conn->outputLight(g,r);
			delay(40);
			g &= ~l1;
			r &= ~l2;
		}
		g |= this->clientIdToBinary(i);
		r |= this->clientIdToBinary(PLAYER_NUMBER - i + 1);
		this->conn->outputLight(g, r);
	}
	r = 0;
	g = 0;
	this->conn->outputLight(g, r);
	for(int i = 1;i<=PLAYER_NUMBER;i++){
		this->conn->setScreen(PLAYER_NUMBER - i);
		for(int j = i ; j <= PLAYER_NUMBER - i + 1;j++){
			int q = PLAYER_NUMBER - j + 1;
			this->conn->setScreen(i + j);
			uint16_t l2 = this->clientIdToBinary(q);
			uint16_t l1 = this->clientIdToBinary(j);
			g |= l1;
			r |= l2;
			this->conn->outputLight(g,r);
			delay(40);
			g &= ~l1;
			r &= ~l2;

		}
		r |= this->clientIdToBinary(i);
		g |= this->clientIdToBinary(PLAYER_NUMBER - i + 1);
		this->conn->outputLight(g, r);
	}
	delay(1000);
	this->powerOnAllLight();
}
