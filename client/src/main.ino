#include <Wire.h>
#include "identity.h"

#define SENSATIVITY 400

// uncommit this to run in debug mode
//#define DEBUG



uint8_t redPin = 12;
uint8_t greenPin = 8;

uint8_t btn = 0;

int JoyStick_X = 1;
int JoyStick_Y = 2;
int JoyStick_Z = 9;

void setup(){
  Wire.begin(CLIENT_ID);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  Serial.begin(115200);
  pinMode(JoyStick_Z, INPUT);
  pinMode(JoyStick_Z,INPUT_PULLUP);

  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);

}

void loop(){
  int x,y,z;
  x=analogRead(JoyStick_X);
  y=analogRead(JoyStick_Y);
  z=digitalRead(JoyStick_Z);
  bool have_input = false;
  if(x > 1020){
    btn = 3;
    have_input = true;
  }else if(x < 10){
    btn = 4;
    have_input = true;
  }

  if(y > 1020){
    btn = 2;
    have_input = true;
  }else if(y < 10){
    btn = 1;
    have_input = true;
  }

  if(z == LOW){
    btn = 5;
    have_input = true;
  }

  if(!have_input){
    btn = 0;
  }
  #ifdef DEBUG
  Serial.println(btn);
  if(Serial.available()){
    int action = Serial.parseInt();
    light(action);
  }
  #endif
}

void requestEvent(){
  Wire.write(btn);
}

void receiveEvent(int numBytes){
  uint8_t lightStatus = Wire.read();
  light(lightStatus);
}
void light(uint8_t lightStatus){
  digitalWrite(greenPin,(lightStatus & 1) > 0);
  digitalWrite(redPin,  (lightStatus & 2) > 0);
}
