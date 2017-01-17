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

String inputString = "";
/********************************************************************************
 * Game Logic
 ********************************************************************************/

uint8_t checkResult(){
  return 0;
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
    Serial.println(String("Selecting:") + select);
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
    if(select >PLAYER_NUMBER){
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
  *clientId = 1;
  *btn = 1;
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
  Serial.println("Init game...");
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
  
  Serial.println("Game init sucessfully!");
  Serial.println("Game Started");
  delay(100);
  g_playerRedLight = 0;
  g_playerGreenLight = 0;
  renderLight();
  
  #ifdef TEST_MODE
  test();
  while(1);
  #endif
}



void loop() {
  gameLoop();
}
void gameLoop(){
  Serial.println("Night is comming, all close eyes...");
  powerOffAllLight();
  delay(3000);
  uint8_t id,btn;
  /**
   * Lycan turn
   */
  Serial.println("Lycan show up...");
  delay(3000);
  if(g_isFirstLoop){
    Serial.println("Lycan confirm identity...");
    uint8_t confirmCount = 0;
    
    while(confirmCount < LYCAN_NUMBER){
      if( getInput(&id,&btn) && (g_playerRole[id - 1] != R_LYCAN)){
        confirmCount ++;
        g_playerRole[id - 1] = R_LYCAN;
        g_playerRedLight += clientIdToBinary(id);
        renderLight();
        Serial.println(String("Lycan confirmed:") + id);
      }
    }
  }
  powerOffAllLight();
  Serial.println("Lycan select to kill...");
  uint8_t lycan_kill_id = selectOne(30000,R_LYCAN);
  Serial.println(String("Lycan select to kill:") + lycan_kill_id);
  delay(3000);
  Serial.println("Lycan close eyes...");
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
    while(1){
      if(getInput(&id,&btn) && (g_playerRole[id - 1] == R_CITIZEN)){
        g_playerRole[id - 1] = R_WITCH;
        g_witchId = id;
        Serial.println("Witch confirmed:" + id);
        g_playerGreenLight = clientIdToBinary(id);
        renderLight();
        delay(2000);
        break;
      }
    }
  }
  powerOffAllLight();
  uint8_t witch_kill_id= 0;
  bool witchDie = (g_playerAlive & clientIdToBinary(g_witchId)) == 0;
  Serial.println(String("Tonight's vicitm is:")+ lycan_kill_id);
  delay(2000);
  if(witchDie){
    Serial.println("Do you want to save this guy?");
    delay(5000);
    Serial.println("Do you want to posion someone?");
    delay(5000);
  }else{
  
    g_playerGreenLight = 0;
    g_playerRedLight = g_witchCureUsed ? 0 : clientIdToBinary(lycan_kill_id);
    renderLight();
    delay(1000);
    Serial.println("Do you want to save this guy?");
    // Save vicitm
    bool saveSuccess = false;
    if(!g_witchCureUsed){
      if(lycan_kill_id == g_witchId){
        if(g_isFirstLoop){
          g_playerGreenLight = clientIdToBinary(lycan_kill_id);
          g_playerRedLight = clientIdToBinary(lycan_kill_id);
          renderLight();
          if(confirm(0,R_WITCH)){
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
        if(confirm(0,R_WITCH)){
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
      delay(5000);
    }else{
      witch_kill_id = selectOne(30000,R_WITCH);
      g_witchPosionUsed = true;
    }
    
  }
  powerOffAllLight();
  /**
   * R_PROPHET turn
   */
  
  
  g_isFirstLoop = false;
}
void test(){
  uint16_t a = 65535;
  g_playerAlive = clientIdToBinary(3) |clientIdToBinary(4) | clientIdToBinary(12) | clientIdToBinary(11) | clientIdToBinary(1) | clientIdToBinary(2)  ;
  g_playerRole[0] = R_LYCAN;
  g_playerRole[1] = R_WITCH;
  Serial.println(a);
  Serial.println("Lycan Select One...");
  uint8_t select = selectOne(30000,R_LYCAN);
  Serial.println(String("You Select:") + select);
  g_playerAlive = g_playerAlive & ~clientIdToBinary(select);
  
  Serial.println("Lycan Select One...");
  select = selectOne(30000,R_LYCAN);
  Serial.println(String("You Select:") + select);
  
  delay(3000);
  Serial.println("Witch Please Confirm...");
  bool result = confirm(0,R_WITCH);
  Serial.println(String("Confirm result:") + result);
  
}

