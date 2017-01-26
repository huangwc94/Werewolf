#include <Wire.h>

#define SENSATIVITY 400;

uint8_t redPin = 12;
uint8_t greenPin = 8;

uint8_t btn = 0;

int JoyStick_X = 1;
int JoyStick_Y = 2;
int JoyStick_Z = 9;

void setup(){
  Wire.begin(1);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  pinMode(redPin,OUTPUT);
  pinMode(greenPin,OUTPUT);
  Serial.begin(115200);
  pinMode(JoyStick_Z, INPUT);
  pinMode(JoyStick_Z,INPUT_PULLUP);

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
  Serial.println(btn);
}

void requestEvent(){
  Wire.write(btn);
}

void receiveEvent(){
  uint8_t lightStatus = Wire.read();
  digitalWrite(greenPin,(lightStatus & 1) > 0);
  digitalWrite(redPin,  (lightStatus & 2) > 0);
}
