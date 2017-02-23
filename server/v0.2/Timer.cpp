#include "Timer.h"

Timer::Timer(unsigned long timeout,Driver * conn){
  this->conn = conn;
  this->timeout = timeout;
  this->start = millis();
  this->current = this->start;
  this->one = true;
  this->two = true;
  this->three = true;
  this->five = true;
  this->ten = true;
  this->thy = true;
  this->conn->setScreen(timeout / 1000);
}
Timer::~Timer(){
  this->conn->setScreen(10000);
}

bool Timer::run(){

  if(timeout == 0){
    this->conn->setScreen(100);
    return true;
  }
  long remaining = timeout + start - current;
  this->conn->setScreen(remaining/1000 + 1);
  if(remaining <= 1000 && one){
    this->conn->playSound(60);
    one = false;
  }else if (remaining <= 2000 && two){
    this->conn->playSound(59);
    two = false;
  }else if (remaining <= 3000 && three){
    this->conn->playSound(58);
    three = false;
  }else if (remaining <= 5000 && five && remaining > 3000){
    this->conn->playSound(63);
    five = false;
  }else if (remaining <= 10000 && ten && remaining > 5000){
    this->conn->playSound(62);
    ten = false;
  }else if (remaining <= 30000 && thy && remaining > 10000){
    thy = false;
    this->conn->playSound(66);
  }
  current = millis();

  return remaining >= 0;
}
