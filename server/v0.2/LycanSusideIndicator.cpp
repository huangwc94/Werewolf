#include "LycanSusideIndicator.h"
#include "Arduino.h"

LycanSusideIndicator::LycanSusideIndicator(GameLogic *gl){

  for(Pid i = 1;i<PLAYER_NUMBER;i++){
    this->lycan_last_time[i-1] = 0;
    this->lycan_last_action[i-1] = 0;
  }
  this->gl = gl;
}
LycanSusideIndicator::~LycanSusideIndicator(){

}
bool LycanSusideIndicator::detect(uint8_t id,uint8_t btn){

  if(!gl->isPlayerAlive(id)) return false;
  if(gl->tstatus->lycanSusideId>0) return false;

  if(btn != 1 && btn != 2){
    this->lycan_last_action[id-1] = 0;
    return false;
  }
  unsigned long current = millis();

  if((current - this->lycan_last_time[id-1]) > SUSIDE_TIME){
    this->lycan_last_action[id-1] = 0;
    this->lycan_last_time[id-1] = current;
  }

  switch (this->lycan_last_action[id-1]) {
    case 0:
      if(btn == 1){
        this->lycan_last_action[id-1] ++;
        this->lycan_last_time[id-1] = current;
      }
      break;
    case 1:
      if(btn == 2){
        this->lycan_last_action[id-1] ++;
        this->lycan_last_time[id-1] = current;
      }
      break;
    case 2:
      if(btn == 1){
        this->lycan_last_action[id-1] ++;
        this->lycan_last_time[id-1] = current;
      }
      break;
    case 3:
      if(btn == 2){
        if(gl->status->playerRole[id - 1] != R_LYCAN){
          this->goodMansuside(id);
          return false;
        }else{
          this->suside(id);
          return true;
        }

      }
      break;
  }
  return false;

}

void LycanSusideIndicator::goodMansuside(Pid gid){

  gl->conn->outputLight(gl->clientIdToBinary(gid), gl->clientIdToBinary(gid));
  gl->conn->playSound(34 + gid);
  gl->say(String(gid) + "号玩家");
  gl->conn->playSound(70);
  gl->say("违反游戏规则，被判定为自杀");
  gl->markPlayerDie(gid);
  gl->powerOffAllLight();
}
void LycanSusideIndicator::suside(Pid lycan){
  gl->conn->playSound(69);
  for(int i = 0;i<6;i++){
    gl->powerOffAllLight();
    delay(200);
    gl->powerOnAllLight();
    delay(200);
  }
  uint16_t l = ~0;
  int left,right;
  int opposite = lycan + PLAYER_NUMBER / 2;

  if(opposite > PLAYER_NUMBER){
    opposite -= PLAYER_NUMBER;
  }
  for(int proc = 0; proc < PLAYER_NUMBER / 2 ;proc++){
    delay(300);
    left = opposite - proc;
    right = opposite + proc;

    if(left < 1){
      left += PLAYER_NUMBER;
    }

    if(right > PLAYER_NUMBER){
      right -= PLAYER_NUMBER;
    }

    l = l & ~(gl->clientIdToBinary(left));
    l = l & ~(gl->clientIdToBinary(right));

    gl->conn->outputLight(l, l);

  }

  gl->conn->playSound(34 + lycan);
  gl->say(String(lycan) + "号玩家");
  gl->tstatus->lycanSusideId = lycan;
  gl->powerOffAllLight();
}
