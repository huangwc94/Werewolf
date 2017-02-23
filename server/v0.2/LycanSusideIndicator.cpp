#include "LycanSusideIndicator.h"
#include "Arduino.h"

LycanSusideIndicator::LycanSusideIndicator(GameLogic *gl){
  Pid index = 0;
  for(Pid i = 1;i<PLAYER_NUMBER;i++){
    if(gl->status->playerRole[i - 1] == R_LYCAN){
      this->lycan_id[index] = i;
      this->lycan_last_time[index] = 0;
      this->lycan_last_action[index] = 0;
      index++;
    }
  }
  this->gl = gl;
}
LycanSusideIndicator::~LycanSusideIndicator(){

}
bool LycanSusideIndicator::detect(uint8_t id,uint8_t btn){

  if(gl->status->playerRole[id - 1] != R_LYCAN) return false;
  if(!gl->isPlayerAlive(id)) return false;
  if(btn != 1 && btn != 2) return false;
  if(gl->tstatus->lycanSusideId>0) return false;

  int this_lycan_id = -1;
  for(int i = 0;i<LYCAN_NUMBER;i++){
    if(this->lycan_id[i] == id){
      this_lycan_id = i;
      break;
    }
  }
  if(this_lycan_id < 0) return false;


  unsigned long current = millis();

  if((current - this->lycan_last_time[this_lycan_id]) > SUSIDE_TIME){
    this->lycan_last_action[this_lycan_id] = 0;
    this->lycan_last_time[this_lycan_id] = current;
  }

  switch (this->lycan_last_action[this_lycan_id]) {
    case 0:
      if(btn == 1){
        this->lycan_last_action[this_lycan_id] ++;
        this->lycan_last_time[this_lycan_id] = current;
      }
      break;
    case 1:
      if(btn == 2){
        this->lycan_last_action[this_lycan_id] ++;
        this->lycan_last_time[this_lycan_id] = current;
      }
      break;
    case 2:
      if(btn == 1){
        this->lycan_last_action[this_lycan_id] ++;
        this->lycan_last_time[this_lycan_id] = current;
      }
      break;
    case 3:
      if(btn == 2){
        this->suside(id);
        return true;
      }
      break;
  }
  return false;

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
